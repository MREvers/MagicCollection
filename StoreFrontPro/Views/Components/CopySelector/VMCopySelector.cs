using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Components.CopySelector
{
   class VMCopySelector : ViewModel<CardModel>
   {
      #region Bindings
      public ObservableCollection<VCopy> Options { get; set; } = new ObservableCollection<VCopy>();
      #endregion

      #region Public Methods
      public VMCopySelector(CardModel Model, string RoutingName)
         : base(Model, RoutingName)
      {
      }

      public void SetEditting(CardModel Model, bool PersistSelection)
      {
         this.Model = Model;
         populateCopies(PersistSelection);
      }

      public IEnumerable<VMCopy> GetSelectedCopyList()
      {
         return Options.Select(x => getVMCopy(x)).Where(x => x.Selected);
      }
      #endregion

      #region Private Methods
      private void populateCopies(bool PersistSelection)
      {
         List<VMCopy> lstSelections = Options.Select(x => getVMCopy(x)).ToList();

         Options.Clear();
         for(int i = 0; i < Model.UIDs.Count; i++)
         {
            string copy = Model.UIDs[i];
            ViewClass copyClass = ViewFactory.CreateCopySelectorCopyItem(copy, copy);
            Options.Add((VCopy)copyClass.View);

            if( i == 0 )
            {
               var copyvm = (VMCopy) copyClass.ViewModel;
               copyvm.Selected = true;
            }
         }
         
         if( PersistSelection )
         {
            foreach(var currentCopy in Options.Select(x => getVMCopy(x)))
            {
               VMCopy matchinCopy = lstSelections.FirstOrDefault(x => x.Specifier == currentCopy.Specifier);
               if( matchinCopy != null )
               {
                  currentCopy.Selected = matchinCopy.Selected;
               }
            }
         }
      }

      private VMCopy getVMCopy(VCopy view)
      {
         return (VMCopy)view.DataContext;
      }
      #endregion

      #region IViewModel
      public override void ModelUpdated()
      {
         throw new NotImplementedException();
      }
      #endregion

   }
}
