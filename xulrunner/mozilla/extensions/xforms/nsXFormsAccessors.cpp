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
 * Novell, Inc.
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Allan Beaufour <abeaufour@novell.com>
 *  Olli Pettay <Olli.Pettay@helsinki.fi>
 *  Alexander Surkov <surkov.alexander@gmail.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
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

#include "nsStringAPI.h"
#include "nsIDOMElement.h"
#include "nsXFormsUtils.h"
#include "nsXFormsAccessors.h"
#include "nsIEventStateManager.h"
#include "nsIContent.h"
#include "nsIXFormsControl.h"
#include "nsIProgrammingLanguage.h"

// nsXFormsAccessors object implementation

NS_IMPL_ISUPPORTS2(nsXFormsAccessorsBase, nsIXFormsAccessors, nsIClassInfo)

nsresult
nsXFormsAccessorsBase::GetState(PRInt32 aState, PRBool *aStateVal)
{
  NS_ENSURE_ARG_POINTER(aStateVal);

  nsCOMPtr<nsIModelElementPrivate> model;
  GetModel(getter_AddRefs(model));
  NS_ENSURE_STATE(model);

  nsCOMPtr<nsIDOMNode> instNode;
  GetInstanceNode(getter_AddRefs(instNode));
  NS_ENSURE_STATE(instNode);

  PRInt32 states;
  model->GetStates(instNode, &states);
  *aStateVal = states & aState;

  return NS_OK;
}

NS_IMETHODIMP
nsXFormsAccessorsBase::GetValue(nsAString &aValue)
{
  nsCOMPtr<nsIDOMNode> instNode;
  GetInstanceNode(getter_AddRefs(instNode));
  NS_ENSURE_STATE(instNode);

  nsXFormsUtils::GetNodeValue(instNode, aValue);

  return NS_OK;
}

NS_IMETHODIMP
nsXFormsAccessorsBase::SetValue(const nsAString &aValue)
{
  nsCOMPtr<nsIModelElementPrivate> model;
  GetModel(getter_AddRefs(model));
  NS_ENSURE_STATE(model);

  nsCOMPtr<nsIDOMNode> instNode;
  GetInstanceNode(getter_AddRefs(instNode));
  NS_ENSURE_STATE(instNode);

  PRBool nodeChanged;
  return model->SetNodeValue(instNode, aValue, PR_TRUE, &nodeChanged);
}

NS_IMETHODIMP
nsXFormsAccessorsBase::HasBoundNode(PRBool *aHasBoundNode)
{
  NS_ENSURE_ARG_POINTER(aHasBoundNode);

  nsCOMPtr<nsIDOMNode> instNode;
  GetInstanceNode(getter_AddRefs(instNode));
  *aHasBoundNode = instNode ? PR_TRUE : PR_FALSE;

  return NS_OK;
}

NS_IMETHODIMP
nsXFormsAccessorsBase::IsReadonly(PRBool *aStateVal)
{
  return GetState(NS_EVENT_STATE_MOZ_READONLY, aStateVal);
}

NS_IMETHODIMP
nsXFormsAccessorsBase::IsRelevant(PRBool *aStateVal)
{
  return GetState(NS_EVENT_STATE_ENABLED, aStateVal);
}

NS_IMETHODIMP
nsXFormsAccessorsBase::IsRequired(PRBool *aStateVal)
{
  return GetState(NS_EVENT_STATE_REQUIRED, aStateVal);
}

NS_IMETHODIMP
nsXFormsAccessorsBase::IsValid(PRBool *aStateVal)
{
  return GetState(NS_EVENT_STATE_VALID, aStateVal);
}

NS_IMETHODIMP
nsXFormsAccessorsBase::SetContent(nsIDOMNode *aNode, PRBool aForceUpdate)
{
  NS_ENSURE_ARG(aNode);

  nsCOMPtr<nsIModelElementPrivate> model;
  GetModel(getter_AddRefs(model));
  NS_ENSURE_STATE(model);

  nsCOMPtr<nsIDOMNode> instNode;
  GetInstanceNode(getter_AddRefs(instNode));
  NS_ENSURE_STATE(instNode);

  return model->SetNodeContent(instNode, aNode, aForceUpdate);
}

NS_IMETHODIMP
nsXFormsAccessorsBase::GetBoundNode(nsIDOMNode **aBoundNode)
{
  return GetInstanceNode(aBoundNode);
}

// nsIClassInfo implementation

static const nsIID sScriptingIIDs[] = {
  NS_IXFORMSACCESSORS_IID
};

NS_IMETHODIMP
nsXFormsAccessorsBase::GetInterfaces(PRUint32 *aCount, nsIID * **aArray)
{
  return nsXFormsUtils::CloneScriptingInterfaces(sScriptingIIDs,
                                                 NS_ARRAY_LENGTH(sScriptingIIDs),
                                                 aCount, aArray);
}

NS_IMETHODIMP
nsXFormsAccessorsBase::GetHelperForLanguage(PRUint32 language,
                                            nsISupports **_retval)
{
  *_retval = nsnull;
  return NS_OK;
}

NS_IMETHODIMP
nsXFormsAccessorsBase::GetContractID(char * *aContractID)
{
  *aContractID = nsnull;
  return NS_OK;
}

NS_IMETHODIMP
nsXFormsAccessorsBase::GetClassDescription(char * *aClassDescription)
{
  *aClassDescription = nsnull;
  return NS_OK;
}

NS_IMETHODIMP
nsXFormsAccessorsBase::GetClassID(nsCID * *aClassID)
{
  *aClassID = nsnull;
  return NS_OK;
}

NS_IMETHODIMP
nsXFormsAccessorsBase::GetImplementationLanguage(PRUint32 *aImplementationLanguage)
{
  *aImplementationLanguage = nsIProgrammingLanguage::CPLUSPLUS;
  return NS_OK;
}

NS_IMETHODIMP
nsXFormsAccessorsBase::GetFlags(PRUint32 *aFlags)
{
  *aFlags = nsIClassInfo::DOM_OBJECT;
  return NS_OK;
}

NS_IMETHODIMP
nsXFormsAccessorsBase::GetClassIDNoAlloc(nsCID *aClassIDNoAlloc)
{
  return NS_ERROR_NOT_AVAILABLE;
}

// nsXFormsAccessors implementation

nsXFormsAccessors::nsXFormsAccessors(nsIModelElementPrivate *aModel,
                                     nsIDOMNode *aInstanceNode)
  : mModel(aModel), mInstanceNode(aInstanceNode)
{
}

nsresult
nsXFormsAccessors::GetModel(nsIModelElementPrivate **aModel)
{
  NS_ENSURE_ARG_POINTER(aModel);

  NS_IF_ADDREF(*aModel = mModel);
  return NS_OK;
}

nsresult
nsXFormsAccessors::GetInstanceNode(nsIDOMNode **aInstanceNode)
{
  NS_ENSURE_ARG_POINTER(aInstanceNode);

  NS_IF_ADDREF(*aInstanceNode = mInstanceNode);
  return NS_OK;
}

// nsXFormsControlAccessors implementation
nsXFormsControlAccessors::nsXFormsControlAccessors(nsIDelegateInternal *aControl,
                                                   nsIDOMElement *aElement)
: mDelegate(aControl), mElement(aElement)
{
}

nsresult
nsXFormsControlAccessors::GetModel(nsIModelElementPrivate **aModel)
{
  NS_ENSURE_ARG_POINTER(aModel);
  NS_ENSURE_STATE(mDelegate);

  nsCOMPtr<nsIXFormsControl> control = do_QueryInterface(mDelegate);
  return control->GetModel(aModel);
}

nsresult
nsXFormsControlAccessors::GetInstanceNode(nsIDOMNode **aInstanceNode)
{
  NS_ENSURE_ARG_POINTER(aInstanceNode);
  NS_ENSURE_STATE(mDelegate);

  nsCOMPtr<nsIXFormsControl> control = do_QueryInterface(mDelegate);
  return control->GetBoundNode(aInstanceNode);
}

nsresult
nsXFormsControlAccessors::GetState(PRInt32 aState, PRBool *aStateVal)
{
  NS_ENSURE_ARG_POINTER(aStateVal);

  if (mDelegate) {
    NS_ENSURE_STATE(mElement);

    nsCOMPtr<nsIContent> content(do_QueryInterface(mElement));
    *aStateVal = (content && (content->IntrinsicState() & aState));
  }

  return NS_OK;
}

NS_IMETHODIMP
nsXFormsControlAccessors::GetValue(nsAString &aValue)
{
  if (mDelegate)
    return mDelegate->GetValue(aValue);

  aValue.SetIsVoid(PR_TRUE);
  return NS_OK;
}

NS_IMETHODIMP
nsXFormsControlAccessors::SetValue(const nsAString &aValue)
{
  return mDelegate ? mDelegate->SetValue(aValue) : NS_OK;
}

void
nsXFormsControlAccessors::Destroy()	
{
  mElement = nsnull;
  mDelegate = nsnull;
}
