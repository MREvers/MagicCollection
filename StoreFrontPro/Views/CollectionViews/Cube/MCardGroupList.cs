using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using StoreFrontPro.Tools;

namespace StoreFrontPro.Views.CollectionViews.Cube
{
   /// <summary>
   /// This class wraps a List<CardModel> model.
   /// </summary>
   class MCardGroupList : IModel, IViewModel
   {
      private List<CardModel> _ungroupedList;
      public List<CardModel> GroupedList
      {
         get { return _ungroupedList; }
      }

      public BasicModel<List<CardModel>> BaseModel;

      public string GroupName;
      public List<string> ExpectedAttributes;
      private string m_szFilteringAttribute;
      private List<string> m_szAttributeSet;

      public MCardGroupList( string FilteredAttribute, string ExpectedAttribute,
                             List<string> AttributeSet, BasicModel<List<CardModel>> BaseList )
      {
         BaseList.Register(this);
         BaseModel = BaseList;
         ExpectedAttributes = ExpectedAttribute.Split('\\').ToList();
         GroupName = ExpectedAttribute;

         m_szFilteringAttribute = FilteredAttribute;
         m_szAttributeSet = AttributeSet;
         ModelUpdated();
      }

      #region Private functions
      // Can eventually use a strategy pattern to decide how to filter. TODO
      private List<CardModel> getFilteredList(List<CardModel> alstList)
      {
         return filterListOnIdentityAndCMC(alstList);
      }

      private List<CardModel> filterListOnIdentityAndCMC(List<CardModel> alstNew)
      {
         List<CardModel> lstRetVal = new List<CardModel>();

         foreach (CardModel oCardModel in alstNew)
         {
            string szKeyAttr = oCardModel.GetAttr(m_szFilteringAttribute);

            bool isNotElement = false;
            foreach (string attr in m_szAttributeSet)
            {
               isNotElement |= ExpectedAttributes.Contains(attr) != szKeyAttr.Contains(attr);
            }

            if (!isNotElement)
            {
               lstRetVal.Add(oCardModel);
            }
         }

         List<CardModel> lstRealRetVal = new List<CardModel>();
         foreach (CardModel cm in lstRetVal.OrderBy(x => CMC(x)).ThenBy(x => x.GetIdealIdentifier()))
         {
            lstRealRetVal.Add(cm);
         }

         return lstRealRetVal;
      }

      private int CMC(CardModel x)
      {
         int iCMC = 0;
         int iNumSpecs = 0;
         string szMC = x.GetAttr("manaCost");
         if (!string.IsNullOrEmpty(szMC))
         {
            int iSpecStart = Math.Max(szMC.IndexOf('}'), 0);
            if (!int.TryParse(szMC.Substring(1, Math.Max(iSpecStart - 1, 0)), out iCMC))
            {
               iCMC = 0;
            }

            iNumSpecs = szMC.Count((c) => c == '{') - 1;
         }

         return iCMC + iNumSpecs;
      }
      #endregion

      #region IModel
      private List<IViewModel> m_lstViewers = new List<IViewModel>();
      public void Register(IViewModel item)
      {
         m_lstViewers.Add(item);
      }

      public void UnRegister(IViewModel item)
      {
         m_lstViewers.Remove(item);
      }

      public void NotifyViewModel()
      {
         m_lstViewers.ForEach(x => x.ModelUpdated());
      }

      public void EnableNotification(bool abNotify)
      {
         throw new NotImplementedException();
      }

      public void DisableNotification()
      {
         throw new NotImplementedException();
      }

      public void Sync(bool ASync = true)
      {
         throw new NotImplementedException();
      }
      #endregion

      #region IViewModel
      public void ModelUpdated()
      {
         _ungroupedList = getFilteredList(BaseModel);
         NotifyViewModel();
      }

      public void FreeModel()
      {
         BaseModel.UnRegister(this);
      }
      #endregion
   }
}
