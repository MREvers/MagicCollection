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
      private Action<bool> syncAction;
      private bool notify;

      public BasicModel(T aItem, Action<bool> aSyncAction)
      {
         Item = aItem;
         syncAction = aSyncAction;
      }

      #region IModel
      private List<WeakReference<IViewModel>> viewModels = new List<WeakReference<IViewModel>>();
      public void NotifyViewModel()
      {
         viewModels.ForEach(x => 
         {
            IViewModel model;
            if(x.TryGetTarget(out model))
            {
               model.ModelUpdated();
            }
         });
      }

      public void Register(IViewModel item)
      {
         viewModels.Add(new WeakReference<IViewModel>(item));
      }

      public void UnRegister(IViewModel item)
      {
         // Find the model that corresponds.
         var lstRemoves = new List<WeakReference<IViewModel>>();
         foreach(var model in viewModels)
         {
            IViewModel test;
            if(model.TryGetTarget(out test))
            {
               if( test == item )
               {
                  lstRemoves.Add(model);
               }
            }
         }

         foreach( var model in lstRemoves )
         {
            viewModels.Remove(model);
         }
      }

      public void Sync(bool ASync = true)
      {
         syncAction?.Invoke(ASync);
         if (notify) { NotifyViewModel(); }
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
      #endregion

      public void Dispose()
      {
         viewModels.Clear();
      }

      public static implicit operator T (BasicModel<T> Model)
      {
         return Model.Item;
      }
   }
}
