using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Support.SortingList
{
   class SortingList
   {
      public class Group
      {
         public string Grouping;
         public IEnumerable<CardModel> GroupList;
         public Group(string aszGrouping, IEnumerable<CardModel> lstGroup)
         {
            Grouping = aszGrouping;
            GroupList = lstGroup;
         }
      }

      private class SortItem
      {
         public string Key;
         public List<string> OnValues;

         public SortItem(string aszKey, string[] lstFitting)
         {
            Key = aszKey;
            OnValues = lstFitting.ToList();
         }
      }

      public List<CardModel> BaseList { get; private set; }
      public bool SortOnFeature = false;

      List<SortItem> m_lstSortList = new List<SortItem>();

      public SortingList(List<CardModel> BaseList)
      {
         this.BaseList = BaseList;
      }

      public List<Group> GetGroups()
      {
         if( m_lstSortList.Count == 0 ) { return new List<Group>(); }

         List<Group> lstGroups = new List<Group>();
         Group lstNoMatch = new Group(m_lstSortList[0].Key + "-less", new List<CardModel>());
         lstGroups.Add(lstNoMatch);

         Dictionary<string, List<CardModel>> dictGroups =
            new Dictionary<string, List<CardModel>>();

         foreach( CardModel model in BaseList )
         {
            string value = getValue(model, m_lstSortList[0]);
            if( value != "" )
            {
               if( dictGroups.ContainsKey(value) )
               {
                  dictGroups[value].Add(model);
               }
               else
               {
                  dictGroups.Add(value, new List<CardModel>() { model });
               }
            }
         }

         if( m_lstSortList.Count > 1 )
         {
            foreach( var key in dictGroups.Keys )
            {
               dictGroups[key].Sort((x,y)=> 
               {
                  string szx = getValue(x, m_lstSortList[1]);
                  string szy = getValue(y, m_lstSortList[1]);
                  return szx.CompareTo(szy);
               });

               lstGroups.Add(new Group(key, dictGroups[key]));
            }
         }

         return lstGroups;
      }

      /// <summary>
      /// Takes up to two sorts.
      /// </summary>
      /// <param name="aszSortOn"></param>
      public void SortOn(string aszSortOn, params string[] lstFitting)
      {
         m_lstSortList.Add(new SortItem(aszSortOn, lstFitting));
      }

      private string getValue(CardModel model, SortItem aszkey)
      {
         string szRetval;
         if( SortOnFeature )
         {
            szRetval = model.GetFeature(aszkey.Key);
         }
         else
         {
            szRetval = model.GetAttr(aszkey.Key);
         }

         foreach( var onVal in aszkey.OnValues )
         {
            if( szRetval.Contains(onVal) )
            {
               szRetval = onVal;
               break;
            }
         }

         return szRetval;
      }
   }
}
