/***************************************************************************
    copyright            : (C) 2002 - 2008 by Scott Wheeler
    email                : wheeler@kde.org
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#include <tbytevectorlist.h>
#include <tmap.h>
#include <tstring.h>
#include <tdebug.h>

#include "oggfile.h"
#include "oggpage.h"
#include "oggpageheader.h"

using namespace TagLib;

class Ogg::File::FilePrivate
{
public:
  FilePrivate() :
    streamSerialNumber(0),
    firstPageHeader(0),
    lastPageHeader(0),
    currentPage(0),
    currentPacketPage(0)
  {
    pages.setAutoDelete(true);
  }

  ~FilePrivate()
  {
    delete firstPageHeader;
    delete lastPageHeader;
  }

  uint streamSerialNumber;
  List<Page *> pages;
  PageHeader *firstPageHeader;
  PageHeader *lastPageHeader;
  std::vector< List<int> > packetToPageMap;
  Map<int, ByteVector> dirtyPackets;
  List<int> dirtyPages;

  //! The current page for the reader -- used by nextPage()
  Page *currentPage;
  //! The current page for the packet parser -- used by packet()
  Page *currentPacketPage;
  //! The packets for the currentPacketPage -- used by packet()
  ByteVectorList currentPackets;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

Ogg::File::~File()
{
  delete d;
}

ByteVector Ogg::File::packet(uint i)
{
  // Check to see if we're called setPacket() for this packet since the last
  // save:

  if(d->dirtyPackets.contains(i))
    return d->dirtyPackets[i];

  // If we haven't indexed the page where the packet we're interested in starts,
  // begin reading pages until we have.

  while(d->packetToPageMap.size() <= i) {
    if(!nextPage()) {
      debug("Ogg::File::packet() -- Could not find the requested packet.");
      return ByteVector::null;
    }
  }

  // Start reading at the first page that contains part (or all) of this packet.
  // If the last read stopped at the packet that we're interested in, don't
  // reread its packet list.  (This should make sequential packet reads fast.)

  uint pageIndex = d->packetToPageMap[i].front();
  if(d->currentPacketPage != d->pages[pageIndex]) {
    d->currentPacketPage = d->pages[pageIndex];
    d->currentPackets = d->currentPacketPage->packets();
  }

  // If the packet is completely contained in the first page that it's in, then
  // just return it now.

  if(d->currentPacketPage->containsPacket(i) & Page::CompletePacket)
    return d->currentPackets[i - d->currentPacketPage->firstPacketIndex()];

  // If the packet is *not* completely contained in the first page that it's a
  // part of then that packet trails off the end of the page.  Continue appending
  // the pages' packet data until we hit a page that either does not end with the
  // packet that we're fetching or where the last packet is complete.

  ByteVector packet = d->currentPackets.back();
  while(d->currentPacketPage->containsPacket(i) & Page::EndsWithPacket &&
        !d->currentPacketPage->header()->lastPacketCompleted())
  {
    pageIndex++;
    if(pageIndex == d->pages.size()) {
      if(!nextPage()) {
        debug("Ogg::File::packet() -- Could not find the requested packet.");
        return ByteVector::null;
      }
    }
    d->currentPacketPage = d->pages[pageIndex];
    d->currentPackets = d->currentPacketPage->packets();
    packet.append(d->currentPackets.front());
  }

  return packet;
}

void Ogg::File::setPacket(uint i, const ByteVector &p)
{
  while(d->packetToPageMap.size() <= i) {
    if(!nextPage()) {
      debug("Ogg::File::setPacket() -- Could not set the requested packet.");
      return;
    }
  }

  List<int>::ConstIterator it = d->packetToPageMap[i].begin();
  for(; it != d->packetToPageMap[i].end(); ++it)
    d->dirtyPages.sortedInsert(*it, true);

  d->dirtyPackets.insert(i, p);
}

const Ogg::PageHeader *Ogg::File::firstPageHeader()
{
  if(d->firstPageHeader)
    return d->firstPageHeader->isValid() ? d->firstPageHeader : 0;

  long firstPageHeaderOffset = find("OggS");

  if(firstPageHeaderOffset < 0)
    return 0;

  d->firstPageHeader = new PageHeader(this, firstPageHeaderOffset);
  return d->firstPageHeader->isValid() ? d->firstPageHeader : 0;
}

const Ogg::PageHeader *Ogg::File::lastPageHeader()
{
  if(d->lastPageHeader)
    return d->lastPageHeader->isValid() ? d->lastPageHeader : 0;

  long lastPageHeaderOffset = rfind("OggS");

  if(lastPageHeaderOffset < 0)
    return 0;

  d->lastPageHeader = new PageHeader(this, lastPageHeaderOffset);
  return d->lastPageHeader->isValid() ? d->lastPageHeader : 0;
}

bool Ogg::File::save()
{
  if(readOnly()) {
    debug("Ogg::File::save() - Cannot save to a read only file.");
    return false;
  }

  List<int> pageGroup;

  for(List<int>::ConstIterator it = d->dirtyPages.begin(); it != d->dirtyPages.end(); ++it) {
    if(!pageGroup.isEmpty() && pageGroup.back() + 1 != *it) {
      writePageGroup(pageGroup);
      pageGroup.clear();
    }
    else
      pageGroup.append(*it);
  }
  writePageGroup(pageGroup);
  d->dirtyPages.clear();
  d->dirtyPackets.clear();

  return true;
}

////////////////////////////////////////////////////////////////////////////////
// protected members
////////////////////////////////////////////////////////////////////////////////

Ogg::File::File() : TagLib::File()
{
  d = new FilePrivate;
}

Ogg::File::File(FileName file) : TagLib::File(file)
{
  d = new FilePrivate;
}

////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////

bool Ogg::File::nextPage()
{
  long nextPageOffset;
  int currentPacket;

  if(d->pages.isEmpty()) {
    currentPacket = 0;
    nextPageOffset = find("OggS");
    if(nextPageOffset < 0)
      return false;
  }
  else {
    if(d->currentPage->header()->lastPageOfStream())
      return false;

    if(d->currentPage->header()->lastPacketCompleted())
      currentPacket = d->currentPage->firstPacketIndex() + d->currentPage->packetCount();
    else
      currentPacket = d->currentPage->firstPacketIndex() + d->currentPage->packetCount() - 1;

    nextPageOffset = d->currentPage->fileOffset() + d->currentPage->size();
  }

  // Read the next page and add it to the page list.

  d->currentPage = new Page(this, nextPageOffset);

  if(!d->currentPage->header()->isValid()) {
    delete d->currentPage;
    d->currentPage = 0;
    return false;
  }

  d->currentPage->setFirstPacketIndex(currentPacket);

  if(d->pages.isEmpty())
    d->streamSerialNumber = d->currentPage->header()->streamSerialNumber();

  d->pages.append(d->currentPage);

  // Loop through the packets in the page that we just read appending the
  // current page number to the packet to page map for each packet.

  for(uint i = 0; i < d->currentPage->packetCount(); i++) {
    uint currentPacket = d->currentPage->firstPacketIndex() + i;
    if(d->packetToPageMap.size() <= currentPacket)
      d->packetToPageMap.push_back(List<int>());
    d->packetToPageMap[currentPacket].append(d->pages.size() - 1);
  }

  return true;
}

Ogg::Page* Ogg::File::getNextPage(long searchOffset)
{
  long pageOffset = searchOffset;

  // Read the next page from this offset
  Page *nextPage = new Page(this, pageOffset);

  if(!nextPage->header()->isValid()) {
    delete nextPage;

    // If it wasn't a valid page, then look for the next page with the
    // OggS marker
    pageOffset = find("OggS", pageOffset + 1);
    if(pageOffset < 0)
      return NULL;

    nextPage = new Page (this, pageOffset);
    if(!nextPage->header()->isValid()) {
      delete nextPage;
      return NULL;
    }
  }

  return nextPage;
}

void Ogg::File::writePageGroup(const List<int> &pageGroup)
{
  if(pageGroup.isEmpty())
    return;

  ByteVectorList packets;

  // If the first page of the group starts with a non-dirty packet, append the
  // partial content of that packet here 
  if(!d->dirtyPackets.contains(d->pages[pageGroup.front()]->firstPacketIndex()))
  {
    packets.append(d->pages[pageGroup.front()]->packets().front());
  }

  int previousPacket = -1;
  int originalSize = 0;

  for(List<int>::ConstIterator it = pageGroup.begin();
      it != pageGroup.end();
      ++it)
  {
    uint firstPacket = d->pages[*it]->firstPacketIndex();
    uint lastPacket = firstPacket + d->pages[*it]->packetCount() - 1;

    List<int>::ConstIterator last = --pageGroup.end();

    for(uint i = firstPacket; i <= lastPacket; i++) {

      if(it == last && i == lastPacket && !d->dirtyPackets.contains(i)) {
        packets.append(d->pages[*it]->packets().back());
      }
      else if(int(i) != previousPacket) {
        previousPacket = i;
        packets.append(packet(i));
      }
    }
    originalSize += d->pages[*it]->size();
  }

  const bool continued =
             d->pages[pageGroup.front()]->header()->firstPacketContinued();
  const bool completed =
             d->pages[pageGroup.back()]->header()->lastPacketCompleted();

  // TODO: This pagination method isn't accurate for what's being done here.
  // This should account for real possibilities like non-aligned packets and
  // such.

  List<Page *> pages = Page::paginate(packets, Page::SinglePagePerGroup,
                                      d->streamSerialNumber, pageGroup.front(),
                                      continued, completed);

  // Step 1:
  //   Replace the old pages from the page group with the new repaginated pages
  ByteVector data;

  // This is where the original page group started in the file
  long offset = d->pages[pageGroup.front()]->fileOffset();

  // Loop over each page in the repaginated list, and append it to our buffer
  for (List<Page *>::Iterator pageIter = pages.begin();
       pageIter != pages.end();
       ++pageIter)
  {
    Page *p = *pageIter;
    data.append(p->render());
  }

  // Write the buffer back out to the same file offset, replacing the
  // originalSize bits there
  insert(data, offset, originalSize);

  // Move the offset past this first rewritten set of pages
  offset += data.size();

  // Step 2:
  //   Now that we're done replacing the page group with the new
  //   repaginated pages, we need to see if the page sequence number
  //   is no longer sequential.  If it isn't, we need to renumber the
  //   remaining pages.  If it is, then we can go ahead and leave
  //   the remaining pages where they are.

  if (pages.back()->header()->pageSequenceNumber() != pageGroup.back()) {
    // The delta is the difference between the last page sequence number in
    // the newly paginated pages and the last page sequence number in the
    // passed in pageGroup
    int seqNumChange =
            pages.back()->header()->pageSequenceNumber() - pageGroup.back();

    // Get the following pages after the pageGroup
    Page *page = getNextPage(offset);
    while (page) {
      // Renumber this page by the seqNumChange delta
      ((Ogg::PageHeader *)page->header())->setPageSequenceNumber(
            page->header()->pageSequenceNumber() + seqNumChange);

      // Write this page back out to the file
      ByteVector data;
      data.append(page->render());
      insert(data, page->fileOffset(), data.size());

      offset += data.size();

      // Get the next page
      page = getNextPage(offset);
    }
  }

  // We just rewrote the entire file without maintaining any of the internal
  // state.  Destroy the internal data structure and let it be recreated
  // on-demand
  delete d;
  d = new FilePrivate;
}
