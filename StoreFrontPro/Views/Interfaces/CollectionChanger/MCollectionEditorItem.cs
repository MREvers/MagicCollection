using StoreFrontPro.Server;
using StoreFrontPro.Views.Components.PlusMinusControl;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Interfaces.CollectionChanger
{
   class MCollectionEditorItem : IModel
   {
      public int Amount
      {
         get { return BoxedAmount?.Value ?? 0; }
      }

      public List<string> SetOptions = new List<string>();
      public string SelectedSet;

      public MPlusMinusControl BoxedAmount;
      public string DisplayText;

      public string FunctionText;

      public MCollectionEditorItem(CollectionDelta aDelta)
      {
         SetOptions = aDelta.SelectionOptions ?? new List<string>() { "" };
         SelectedSet = SetOptions[0];
         BoxedAmount = new MPlusMinusControl(1, aDelta.MaxRemoveCount);
         this.DisplayText = aDelta.DisplayString;
         this.FunctionText = aDelta.Command;
      }

      #region IModel
      public void Register(IViewModel item)
      {
         throw new NotImplementedException();
      }

      public void UnRegister(IViewModel item)
      {
         throw new NotImplementedException();
      }

      public void NotifyViewModel()
      {
         throw new NotImplementedException();
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
   }
}
