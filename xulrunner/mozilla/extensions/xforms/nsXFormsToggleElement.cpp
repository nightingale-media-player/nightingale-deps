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
 * The Original Code is Mozilla XForms support.
 *
 * The Initial Developer of the Original Code is
 * Olli Pettay.
 * Portions created by the Initial Developer are Copyright (C) 2004
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Olli Pettay <Olli.Pettay@helsinki.fi> (original author)
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

#include "nsXFormsActionModuleBase.h"
#include "nsIDOMDocument.h"
#include "nsIDOMEvent.h"
#include "nsIDOMElement.h"
#include "nsStringAPI.h"
#include "nsXFormsUtils.h"
#include "nsIXFormsSwitchElement.h"

/**
 * XForms Toggle element. According to the the XForms Schema it can be handled
 * like Action Module elements.
 */
class nsXFormsToggleElement : public nsXFormsActionModuleBase
{
public:
  nsXFormsToggleElement();
  virtual nsresult HandleSingleAction(nsIDOMEvent* aEvent,
                                      nsIXFormsActionElement *aParentAction);
};

nsXFormsToggleElement::nsXFormsToggleElement()
{
}

nsresult
nsXFormsToggleElement::HandleSingleAction(nsIDOMEvent* aEvent,
                                          nsIXFormsActionElement *aParentAction)
{
  // The case's ID can come from the value attribute of the child
  // 'case' element, the string-value of the child 'case'
  // element, or the case attribute of the 'toggle'
  // element. Precedence is in that order.

  nsAutoString caseValue;
  nsCOMPtr<nsIDOMNode> caseNode, currentNode, tmpNode;
  PRUint16 nodeType;
  nsresult rv;

  mElement->GetFirstChild(getter_AddRefs(currentNode));
  while (currentNode) {
    currentNode->GetNodeType(&nodeType);
    if (nodeType == nsIDOMNode::ELEMENT_NODE) {
      // Check to see if it's a case element.
      nsAutoString localName, namespaceURI;
      currentNode->GetLocalName(localName);
      currentNode->GetNamespaceURI(namespaceURI);
      if (localName.EqualsLiteral("case") &&
          namespaceURI.EqualsLiteral(NS_NAMESPACE_XFORMS)) {
        caseNode = currentNode;
        break;
      }
    }
    currentNode->GetNextSibling(getter_AddRefs(tmpNode));
    currentNode.swap(tmpNode);
  }

  if (caseNode) {
    nsCOMPtr<nsIDOMElement> caseElement(do_QueryInterface(caseNode));
    if (caseElement) {
      nsAutoString value;
      caseElement->GetAttribute(NS_LITERAL_STRING("value"), value);
      if (!value.IsEmpty()) {
        // The case ID is given by the result of evaluating the
        // value attribute.
        nsCOMPtr<nsIModelElementPrivate> model;
        nsCOMPtr<nsIDOMXPathResult> xpRes;
        PRBool usesModelBind = PR_FALSE;
        rv = nsXFormsUtils::EvaluateNodeBinding(caseElement, 0,
                                                NS_LITERAL_STRING("value"),
                                                EmptyString(),
                                                nsIDOMXPathResult::STRING_TYPE,
                                                getter_AddRefs(model),
                                                getter_AddRefs(xpRes),
                                                &usesModelBind);
        NS_ENSURE_SUCCESS(rv, rv);
        if (xpRes) {
          rv = xpRes->GetStringValue(caseValue);
          NS_ENSURE_SUCCESS(rv, rv);
        }
      }
      else {
        // Check the string content of the element.
        nsXFormsUtils::GetNodeValue(caseNode, caseValue);
      }
    }
  }
  else {
    // The case ID must be specified on the toggle element.
    mElement->GetAttribute(NS_LITERAL_STRING("case"), caseValue);
  }

  if (caseValue.IsEmpty()) {
    return NS_OK;
  }

  nsCOMPtr<nsIDOMElement> caseEl;
  nsXFormsUtils::GetElementById(caseValue, PR_TRUE, mElement,
                                getter_AddRefs(caseEl));
  if (!caseEl)
    return NS_OK;

  if (!nsXFormsUtils::IsXFormsElement(caseEl, NS_LITERAL_STRING("case")))
    return NS_OK;

  nsCOMPtr<nsIDOMNode> parent;
  caseEl->GetParentNode(getter_AddRefs(parent));
  nsCOMPtr<nsIXFormsSwitchElement> switchEl(do_QueryInterface(parent));
  if (switchEl)
    switchEl->SetSelected(caseEl, PR_TRUE);
  return NS_OK;
}

NS_HIDDEN_(nsresult)
NS_NewXFormsToggleElement(nsIXTFElement **aResult)
{
  *aResult = new nsXFormsToggleElement();
  if (!*aResult)
    return NS_ERROR_OUT_OF_MEMORY;

  NS_ADDREF(*aResult);
  return NS_OK;
}
