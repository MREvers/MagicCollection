using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views
{
   class BasicModel<T>: IModel, IDisposable
   {
      public T Item;
      private List<IViewModel> viewModels = new List<IViewModel>();
      private Action<bool> syncAction;
      private bool notify;

      public BasicModel(T aItem, Action<bool> aSyncAction)
      {
         Item = aItem;
         syncAction = aSyncAction;
      }

      public void NotifyViewModel()
      {
         viewModels.ForEach(x => x.ModelUpdated());
      }

      public void Register(IViewModel item)
      {
         viewModels.Add(item);
      }

      public void UnRegister(IViewModel item)
      {
         viewModels.Remove(item);
      }

      public void Sync(bool ASync = true)
      {
         syncAction?.Invoke(ASync);
         if (notify) { NotifyViewModel(); }
      }

      public void Dispose()
      {
         viewModels.Clear();
      }

      public void EnableNotification(bool abNotify = false)
      {
         notify = true;
         if (abNotify) { NotifyViewModel(); }
      }

      public void DisableNotification()
      {
         notify = false;
      }

      public static implicit operator T (BasicModel<T> Model)
      {
         return Model.Item;
      }
   }
}
