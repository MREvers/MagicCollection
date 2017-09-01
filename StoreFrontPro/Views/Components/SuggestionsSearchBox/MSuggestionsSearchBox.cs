using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Components.SuggestionsSearchBox
{
   class MSuggestionsSearchBox : IModel
   {
      private string _CurrentSearchName = "";
      public string CurrentSearchName
      {
         get { return _CurrentSearchName; }
         set { _CurrentSearchName = value;  NotifyViewModel(); }
      }

      public string ActionButtonName;

      private Func<string, List<string>> m_fnSearchCollection = null;

      public MSuggestionsSearchBox(string ActionName, Func<string, List<string>> SearchCollection)
      {
         m_fnSearchCollection = SearchCollection;
         this.ActionButtonName = ActionName;
      }

      public List<string> GetMatchingCollectionItems(string aszSearch)
      {
         return m_fnSearchCollection.Invoke(aszSearch);
      }

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

      public void Sync(bool ASync = true)
      {

      }

      public void EnableNotification(bool abNotify)
      {
         throw new NotImplementedException();
      }

      public void DisableNotification()
      {
         throw new NotImplementedException();
      }
      #endregion
   }
}
