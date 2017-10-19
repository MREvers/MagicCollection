using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Interfaces.CardInterface
{
   class MAttributeEditorItem : IModel
   {
      public List<string> Options { get; private set; }

      // These fields should only be modified coming from the UI
      // Programmitically, use the setters.
      public string _Name;
      public string Name
      {
         get { return _Name; }
         set { _Name = value; NotifyViewModel(); }
      }

      private string _SelectedOption;
      public string SelectedOption
      {
         get { return _SelectedOption; }
         set { _SelectedOption = value; NotifyViewModel(); }
      }
      
      public MAttributeEditorItem(string TraitName, List<string> Options)
      {
         Name = TraitName;
         this.Options = Options;
         SelectedOption = Options[0];
      }

      #region IModel
      private bool m_bNotify = true;
      private List<WeakReference<IViewModel>> viewModels = new List<WeakReference<IViewModel>>();
      public void NotifyViewModel()
      {
         if( !m_bNotify ) { return; }
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
      }

      public void EnableNotification(bool abNotify = true)
      {
         m_bNotify = abNotify;
      }

      public void DisableNotification()
      {
         m_bNotify = false;
      }
      #endregion
   }
}
