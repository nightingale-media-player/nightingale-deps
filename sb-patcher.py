#!/usr/bin/env python

import sys, os, re, subprocess, xml.etree.ElementTree, optparse

gBaseURL = None
gOutdir = None


class Revision:
  def __init__(self, id, date, author, message):
    self.id = id
    self.date = date
    self.author = author
    self.message = re.sub('[\r\n]+$', '', message)
  def __getitem__(self, key):
    if key == "id":
      return self.id
    elif key == "date":
      return self.date
    elif key == "author":
      return self.author
    elif key == "message":
      return self.message
    else:
      raise KeyError("Unknown key %s" % key)


def run_svn(args):
  print "Running SVN:", ["svn"] + args
  process = subprocess.Popen(["svn"] + args, stdout=subprocess.PIPE)
  output = process.communicate()[0]
  if process.returncode:
    raise Exception("SVN exited with error code %i" % process.returncode)

  return output


def get_base_url():
  output = run_svn(["info", "--xml"])
  root = xml.etree.ElementTree.XML(output)
  result = root.find("entry/url").text
  if not result.endswith("/"):
    result += "/"
  return result


def get_files(cmdArgs):
  global gBaseURL

  output = run_svn(["propget", "--xml", "sb-patch"] + cmdArgs)
  root = xml.etree.ElementTree.XML(output)
  entries = root.findall("target")

  result = {}
  for entry in entries:
    file = entry.attrib["path"]
    if file.startswith(gBaseURL):
      file = file[len(gBaseURL):]
    if file not in result:
      result[file] = []

    patches = re.split('[\r\n]+', entry.find("property").text)
    for patch in patches:
      if not patch:
        continue

      # Sanitize patch name but keep the revision number (if present) out of it
      patch = re.sub('\.patch$', '', patch)
      revision = ""

      fixedRevision = re.match('r(\d+):(.*)', patch)
      if fixedRevision:
        revision = fixedRevision.group(1)
        patch = fixedRevision.group(2)

      patch = re.sub('[^\w\.\-]', '_', patch)

      if revision:
        patch = "r" + revision + ":" + patch

      result[file].append(patch)

  return result


def extract_fixed_revisions(files, patches):
  for file, patchList in files.iteritems():
    for patch in patchList:
      fixedRevision = re.match('r(\d+):(.*)', patch)
      if fixedRevision and fixedRevision.group(2) not in patches:
        patch = fixedRevision.group(2)
        log = get_changelog(["-r", fixedRevision.group(1)])
        if len(log) > 0:
          revision = log[0]
        else:
          revision = Revision(fixedRevision.group(1),
                              "unknown",
                              "unknown",
                              "unknown")
        set_patch_revision(patches, patch, revision)


def remove_known_patches(files, patches):
  delete = []
  for file, patchList in files.iteritems():
    unknownPatches = []
    for patch in patchList:
      patch = re.sub('^r(\d+):', '', patch)
      if patch not in patches:
        unknownPatches.append(patch)

    if unknownPatches:
      files[file] = unknownPatches
    else:
      delete.append(file)

  for file in delete:
    del files[file]


def get_changelog(cmdArgs):
  output = run_svn(["log", "--xml"] + cmdArgs)
  root = xml.etree.ElementTree.XML(output)
  entries = root.findall("logentry")

  result = []
  for entry in entries:
    revision = Revision(entry.attrib["revision"],
                        entry.find("date").text,
                        entry.find("author").text,
                        entry.find("msg").text)
    result.append(revision)
  return result


def diff_patches(patchListBefore, patchListAfter, revision, patches):
  for patch in patchListAfter:
    if patch not in patchListBefore:
      set_patch_revision(patches, patch, revision)


def set_patch_revision(patches, patch, revision):
  global gOutdir

  if patch in patches:
    return

  patches[patch] = revision
  save_revision(revision, os.path.join(gOutdir, patch + ".patch"))


def save_revision(revision, file):
  output = run_svn(["diff", "-c", revision.id])
  f = open(file, "wb")
  f.write("""# SVN changeset patch
# User %(author)s
# Date %(date)s
# Revision ID %(id)s
%(message)s

""" % revision)
  f.write(output)
  f.close()


def extract_patches(dir):
  global gBaseURL, gOutdir

  gBaseURL = get_base_url()

  gOutdir = dir
  if not os.access(gOutdir, os.F_OK):
    os.makedirs(gOutdir, 0755)

  # Get list of patches and patched files in the current revision
  patches = {}
  files = get_files(["-R", "."])
  extract_fixed_revisions(files, patches)
  remove_known_patches(files, patches)

  # Go through the changelog looking for revisions associated with the patches
  while files:
    file = files.keys()[0]
    log = get_changelog([file])
    prevRevision = log[0]
    for revision in log[1:]:
      revisionFiles = get_files(["-r", revision.id, file])
      if file not in revisionFiles:
        revisionFiles[file] = []
      diff_patches(revisionFiles[file], files[file], prevRevision, patches)
      remove_known_patches(files, patches)

      if file not in files:
        break

      prevRevision = revision

    if file in files:
      diff_patches([], files[file], prevRevision, patches)
      remove_known_patches(files, patches)


def check_patches(dir):
  revisions = {}
  for file in os.listdir(dir):
    if not re.search('\.patch$', file):
      continue

    f = open(os.path.join(dir, file), "rb")
    header = f.read(4096)
    f.close()

    revision = re.search('^# Revision ID (\d+)', header, re.MULTILINE)
    if revision:
      if revision.group(1) in revisions:
        print >>sys.stderr, "Patch %s claims the same revision as patch %s" % (file, revisions[revision.group(1)])
      else:
        revisions[revision.group(1)] = file
    else:
      print >>sys.stderr, "Failed to get revision ID for patch %s" % file

    if header.find("Index: ") < 0:
      print >>sys.stderr, "Patch %s doesn't seem to contain any file changes, property-only checkin?" % file

  baseURL = get_base_url()
  log = get_changelog([baseURL])
  for revision in log:
    if revision.id not in revisions:
      print >>sys.stderr, "Unclaimed revision r%(id)s:\n%(author)s %(date)s\n%(message)s\n" % revision


def main(args):
  o = optparse.OptionParser()
  o.add_option("-x", "--extract", action="store_true", dest="doExtract",
               help="Extract patches from SVN checkout in current directory")
  o.add_option("-c", "--check", action="store_true", dest="doCheck",
               help="Check extracted patches for SVN checkout in current directory")
  o.add_option("-d", "--patch-dir", dest="patchdir", default=".",
               help="Patch files directory, default is current directory")
  (options, argv) = o.parse_args()

  if not options.doExtract and not options.doCheck:
    o.print_help()
    return -1

  if options.doExtract:
    extract_patches(options.patchdir)
  if options.doCheck:
    check_patches(options.patchdir)


if __name__ == "__main__":
  main(sys.argv[1:])
