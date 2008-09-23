/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Communicator client code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Original Author: Eric J. Burley (ericb@neoplanet.com)
 *   Ben Turner <mozilla@songbirdnest.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "nsCOMPtr.h"
#include "nsResizerFrame.h"
#include "nsIContent.h"
#include "nsIDocument.h"
#include "nsIDOMXULDocument.h"
#include "nsIDOMNodeList.h"
#include "nsGkAtoms.h"
#include "nsINameSpaceManager.h"

#include "nsIWidget.h"
#include "nsPresContext.h"
#include "nsIDocShell.h"
#include "nsIDocShellTreeItem.h"
#include "nsIDocShellTreeOwner.h"
#include "nsIBaseWindow.h"
#include "nsPIDOMWindow.h"
#include "nsGUIEvent.h"
#include "nsEventDispatcher.h"
#include "nsIPresShell.h"

//
// NS_NewResizerFrame
//
// Creates a new Resizer frame and returns it
//
nsIFrame*
NS_NewResizerFrame(nsIPresShell* aPresShell, nsStyleContext* aContext)
{
  return new (aPresShell) nsResizerFrame(aPresShell, aContext);
} // NS_NewResizerFrame

nsResizerFrame::nsResizerFrame(nsIPresShell* aPresShell, nsStyleContext* aContext)
: nsTitleBarFrame(aPresShell, aContext)
{
  mDirection = topleft; // by default...
}

NS_IMETHODIMP
nsResizerFrame::Init(nsIContent*      aContent,
                     nsIFrame*        aParent,
                     nsIFrame*        asPrevInFlow)
{
  nsresult rv = nsTitleBarFrame::Init(aContent, aParent, asPrevInFlow);
  
  PRBool succeeded;
  
  succeeded = GetInitialDirection(mDirection);
  NS_WARN_IF_FALSE(succeeded, "GetInitialDirection failed!");
  
  GetInitialDirection(mDirection);

  return rv;
}


NS_IMETHODIMP
nsResizerFrame::HandleEvent(nsPresContext* aPresContext,
                            nsGUIEvent* aEvent,
                            nsEventStatus* aEventStatus)
{
  nsWeakFrame weakFrame(this);
  PRBool doDefault = PR_TRUE;

  switch (aEvent->message) {

   case NS_MOUSE_BUTTON_DOWN: {
       if (aEvent->eventStructType == NS_MOUSE_EVENT &&
           static_cast<nsMouseEvent*>(aEvent)->button ==
             nsMouseEvent::eLeftButton)
       {
         nsresult rv = NS_OK;

         // Prevent default processing
         doDefault = PR_FALSE;

         // what direction should we go in? 
         // convert eDirection to horizontal and vertical directions
         static const PRInt8 directions[][2] = {
           {-1, -1}, {0, -1}, {1, -1},
           {-1,  0},          {1,  0},
           {-1,  1}, {0,  1}, {1,  1}
         };

         if (!IsDisabled(aPresContext)) {

           // ask the widget implementation to begin a resize drag if it can
           rv = aEvent->widget->BeginResizeDrag(aEvent, 
               directions[mDirection][0], directions[mDirection][1]);
 
           if (rv == NS_ERROR_NOT_IMPLEMENTED) {
             // there's no native resize support, 
             // we need to window resizing ourselves


             rv = GetWindowFromPresContext(aPresContext,
                                           getter_AddRefs(mResizingWindow));
             NS_ENSURE_SUCCESS(rv, rv);
 
             // we're tracking.
             mTrackingMouseMove = PR_TRUE;
 
             // start capture.
             aEvent->widget->CaptureMouse(PR_TRUE);
             CaptureMouseEvents(aPresContext, PR_TRUE);
 
             // remember current mouse coordinates.
             mLastPoint = aEvent->refPoint;
           }
 
           *aEventStatus = nsEventStatus_eConsumeNoDefault;
         }
       }
     }
     break;


   case NS_MOUSE_BUTTON_UP: {

       if (mTrackingMouseMove && aEvent->eventStructType == NS_MOUSE_EVENT &&
           static_cast<nsMouseEvent*>(aEvent)->button ==
             nsMouseEvent::eLeftButton)
       {
         // we're done tracking.
         mTrackingMouseMove = PR_FALSE;

         // Clear all cached data
         mResizingWindow = nsnull;
         mSizeConstraints.minWidth = SizeConstraints::MIN_CONSTRAINT;
         mSizeConstraints.maxWidth = SizeConstraints::MAX_CONSTRAINT;
         mSizeConstraints.minHeight = SizeConstraints::MIN_CONSTRAINT;
         mSizeConstraints.maxHeight = SizeConstraints::MAX_CONSTRAINT;
         
         // end capture
         aEvent->widget->CaptureMouse(PR_FALSE);
         CaptureMouseEvents(aPresContext, PR_FALSE);

         *aEventStatus = nsEventStatus_eConsumeNoDefault;
         doDefault = PR_FALSE;
       }
     }
     break;

   case NS_MOUSE_MOVE: {
       if (mTrackingMouseMove && !IsDisabled(aPresContext))
       {
         NS_ENSURE_STATE(mResizingWindow);

         nsPoint moveBy(0,0), sizeBy(0,0);
         nsPoint beyondMaximum(0,0), beyondMinimum(0,0);
         nsPoint mouseMove(aEvent->refPoint - mLastPoint);

         switch (mDirection)
         {
            case topleft:
              moveBy = mouseMove;
              sizeBy -= mouseMove;
              break;
            case top:
              moveBy.y = mouseMove.y;
              sizeBy.y = - mouseMove.y;
              break;
            case topright:
              moveBy.y = mouseMove.y;
              sizeBy.x = mouseMove.x;
              mLastPoint.x += mouseMove.x;
              sizeBy.y = -mouseMove.y;
              break;
            case left:
              moveBy.x = mouseMove.x;
              sizeBy.x = -mouseMove.x;
              break;
            case right:
              sizeBy.x = mouseMove.x;
              mLastPoint.x += mouseMove.x;
              break;
            case bottomleft:
              moveBy.x = mouseMove.x;
              sizeBy.y = mouseMove.y;
              sizeBy.x = -mouseMove.x;
              mLastPoint.y += mouseMove.y;
              break;
            case bottom:
              sizeBy.y = mouseMove.y;
              mLastPoint.y += mouseMove.y;
              break;
            case bottomright:
              sizeBy = mouseMove;
              mLastPoint += mouseMove;
              break;
         }
         
         // Remember this because sizeBy is about to be changed
         nsPoint requestedSizeChange(sizeBy);
         
         // The current size of the window is needed for these calculations
         PRInt32 x, y, width, height;
         mResizingWindow->GetPositionAndSize(&x, &y, &width, &height);
         
         // Figure out the size constraints
         nsresult rv;
         nsCOMPtr<nsIWidget> mainWidget;
         rv = mResizingWindow->GetMainWidget(getter_AddRefs(mainWidget));
         NS_ENSURE_SUCCESS(rv, rv);
         
         rv = mainWidget->GetSizeConstraints(&mSizeConstraints.minWidth,
                                             &mSizeConstraints.maxWidth,
                                             &mSizeConstraints.minHeight,
                                             &mSizeConstraints.maxHeight);
         NS_ENSURE_SUCCESS(rv, rv);
         
         // It would be nice to simply pass the new size on to the window but
         // we have to have a way to keep the window from moving
         // inappropriately when size constraints are in effect. Also platform
         // support for size constraints may not exist everywhere, so we go on
         // and do all the necessary calculations here. This will make resizers
         // support size constraints even if native window decorations don't.
	
         // Check if a width constraint is set
         PRInt32 newWidth = width + requestedSizeChange.x;
         if (newWidth < mSizeConstraints.minWidth ||
             newWidth > mSizeConstraints.maxWidth) {
           // Always fix the size
           newWidth = PR_MAX(mSizeConstraints.minWidth,
                             PR_MIN(mSizeConstraints.maxWidth, newWidth));
           sizeBy.x = newWidth - width;
           if (mDirection == topleft || mDirection == left || mDirection == bottomleft) {
             // Move as far as possible
             moveBy.x = width - newWidth;
           }
           else {
             // Track the distance beyond the minimum
             beyondMinimum.x = PR_ABS(newWidth - (width + requestedSizeChange.x));
             // Don't move at all
             moveBy.x = 0;
           }
         }
         
         // Check if a height constraint is set
         PRInt32 newHeight = height + requestedSizeChange.y;
         if (newHeight < mSizeConstraints.minHeight ||
             newHeight > mSizeConstraints.maxHeight) {
           // Always fix the size
           newHeight = PR_MAX(mSizeConstraints.minHeight,
                              PR_MIN(mSizeConstraints.maxHeight, newHeight));
           sizeBy.y = newHeight - height;
           if (mDirection == topleft || mDirection == top || mDirection == topright) {
             // Move as far as possible
             moveBy.y = height - newHeight;
           }
           else {
             // Track the distance beyond the minimum
             beyondMinimum.y = PR_ABS(newHeight - (height + requestedSizeChange.y));
             // Don't move at all
             moveBy.y = 0;
           }
         }
	
         // Update all the variables
         x += moveBy.x;
         y += moveBy.y;
         width += sizeBy.x;
         height += sizeBy.y;

         // Update our tracking point
         mLastPoint += beyondMinimum - beyondMaximum;

         // Do the repaint
         mResizingWindow->SetPositionAndSize(x, y, width, height, PR_TRUE);
         *aEventStatus = nsEventStatus_eConsumeNoDefault;
       }
       doDefault = PR_FALSE;
     }
     break;



    case NS_MOUSE_CLICK:
      if (NS_IS_MOUSE_LEFT_CLICK(aEvent) && !IsDisabled(aPresContext))
      {
        MouseClicked(aPresContext, aEvent);
      }
      break;
  }

  if (doDefault && weakFrame.IsAlive())
    return nsTitleBarFrame::HandleEvent(aPresContext, aEvent, aEventStatus);

  return NS_OK;
}



/* returns true if aText represented a valid direction
 */
PRBool
nsResizerFrame::EvalDirection(nsAutoString& aText, eDirection& aDir)
{
  PRBool aResult = PR_TRUE;

  if( aText.Equals( NS_LITERAL_STRING("topleft") ) )
  {
    aDir = topleft;
  }
  else if( aText.Equals( NS_LITERAL_STRING("top") ) )
  {
    aDir = top;
  }
  else if( aText.Equals( NS_LITERAL_STRING("topright") ) )
  {
    aDir = topright;
  }
  else if( aText.Equals( NS_LITERAL_STRING("left") ) )
  {
    aDir = left;
  }
  else if( aText.Equals( NS_LITERAL_STRING("right") ) )
  {
    aDir = right;
  }
  else if( aText.Equals( NS_LITERAL_STRING("bottomleft") ) )
  {
    aDir = bottomleft;
  }
  else if( aText.Equals( NS_LITERAL_STRING("bottom") ) )
  {
    aDir = bottom;
  }
  else if( aText.Equals( NS_LITERAL_STRING("bottomright") ) )
  {
    aDir = bottomright;
  }
  else
  {
    aResult = PR_FALSE;
  }

  return aResult;
}


/* Returns true if it was set.
 */
PRBool
nsResizerFrame::GetInitialDirection(eDirection& aDirection)
{
 // see what kind of resizer we are.
  nsAutoString value;

  if (!GetContent())
     return PR_FALSE;

  if (GetContent()->GetAttr(kNameSpaceID_None, nsGkAtoms::dir, value)) {
     return EvalDirection(value,aDirection);
  }

  return PR_FALSE;
}


NS_IMETHODIMP
nsResizerFrame::AttributeChanged(PRInt32 aNameSpaceID,
                                 nsIAtom* aAttribute,
                                 PRInt32 aModType)
{
  nsresult rv = nsTitleBarFrame::AttributeChanged(aNameSpaceID, aAttribute,
                                                  aModType);

  if (aAttribute == nsGkAtoms::dir) {
    GetInitialDirection(mDirection);
  }

  return rv;
}


void
nsResizerFrame::MouseClicked(nsPresContext* aPresContext, nsGUIEvent *aEvent)
{
  // Execute the oncommand event handler.
  nsEventStatus status = nsEventStatus_eIgnore;

  nsXULCommandEvent event(aEvent ? NS_IS_TRUSTED_EVENT(aEvent) : PR_FALSE,
                          NS_XUL_COMMAND, nsnull);

  nsEventDispatcher::Dispatch(mContent, aPresContext, &event, nsnull, &status);
}


nsresult
nsResizerFrame::GetWindowFromPresContext(nsPresContext* aPresContext,
                                         nsIBaseWindow** _retval)
{
  NS_ENSURE_ARG_POINTER(aPresContext);
  NS_ENSURE_ARG_POINTER(_retval);

  nsIPresShell* presShell = aPresContext->PresShell();
  NS_ENSURE_STATE(presShell);

  nsIDocument* document = presShell->GetDocument();
  NS_ENSURE_STATE(document);

  nsPIDOMWindow* domWindow = document->GetWindow();
  NS_ENSURE_STATE(domWindow);

  nsIDocShell* docShell = domWindow->GetDocShell();
  NS_ENSURE_STATE(docShell);

  nsresult rv;
  nsCOMPtr<nsIDocShellTreeItem> docShellAsItem =
    do_QueryInterface(docShell, &rv);
  NS_ENSURE_SUCCESS(rv, rv);

  nsCOMPtr<nsIDocShellTreeOwner> treeOwner;
  rv = docShellAsItem->GetTreeOwner(getter_AddRefs(treeOwner));
  NS_ENSURE_SUCCESS(rv, rv);

  nsCOMPtr<nsIBaseWindow> baseWindow = do_QueryInterface(treeOwner, &rv);
  NS_ENSURE_SUCCESS(rv, rv);

  NS_ADDREF(*_retval = baseWindow);
  return NS_OK;
}

PRBool
nsResizerFrame::IsDisabled(nsPresContext* aPresContext)
{
  PRBool isDisabled = PR_FALSE;
  if (GetContent())
    isDisabled = GetContent()->AttrValueIs(kNameSpaceID_None, nsGkAtoms::disabled,
                                           nsGkAtoms::_true, eCaseMatters);

  if (isDisabled)
    return PR_TRUE;

  // Resizers shouldn't be active if the window is maximized
  nsresult rv;
  nsCOMPtr<nsIBaseWindow> baseWindow = mResizingWindow;
  if (!baseWindow) {
    rv = GetWindowFromPresContext(aPresContext, getter_AddRefs(baseWindow));
    NS_ENSURE_SUCCESS(rv, PR_FALSE);
  }

  nsCOMPtr<nsIWidget> mainWidget;
  rv = baseWindow->GetMainWidget(getter_AddRefs(mainWidget));
  NS_ENSURE_SUCCESS(rv, PR_FALSE);

  PRInt32 sizeMode;
  rv = mainWidget->GetSizeMode(&sizeMode);
  NS_ENSURE_SUCCESS(rv, PR_FALSE);

  return sizeMode == nsSizeMode_Maximized;
}
