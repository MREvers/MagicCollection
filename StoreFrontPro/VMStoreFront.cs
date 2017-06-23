using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views;
using StoreFrontPro.Views.CollectionsOverview;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace StoreFrontPro
{
    class VMStoreFront : ViewModel<StoreFront>
    {
        private MultiDisplay _OperationWindow = new MultiDisplay();
        public MultiDisplay OperationWindow
        {
            get { return _OperationWindow; }
            set { _OperationWindow = value; OnPropertyChanged(); }
        }

        public VMStoreFront(StoreFront Model) : base(Model)
        {
            VMCollectionsOverview collectionsOverviewVM = new VMCollectionsOverview(Model.Collections);
            OperationWindow.SetNewDisplay(
                Name: "Overview",
                NewDisplay: new VCollectionsOverview(),
                DataContext: collectionsOverviewVM,
                Persist: false);
            // Should be hooked onto OPERATION window events
        }

        public void Notify()
        {
            // do other updates here

            notifyMultiDisplay();
        }

        private void notifyMultiDisplay()
        {
            UserControl viewMode = OperationWindow.Display;

            // Determine what display mode the multidisplay is in.
            if (viewMode.GetType() == typeof(VCollectionsOverview))
            {
                VMCollectionsOverview collectionsOverviewVM = OperationWindow.DisplayViewModel as VMCollectionsOverview;
                collectionsOverviewVM.SyncWithModel();
            }
        }
    }
}
