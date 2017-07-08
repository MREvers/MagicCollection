using StoreFrontPro.Server;
using StoreFrontPro.Tools;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace StoreFrontPro.Views.CollectionViews.Deckbox
{
    class VMFancyCollectionList : ViewModel<CollectionModel>
    {
        public ObservableCollection<VFancyCollectionItem> Cards { get; set; } = new ObservableCollection<VFancyCollectionItem>();

        private bool _CollapsedView = false;
        private bool CollapsedView
        {
            get
            {
                return _CollapsedView;
            }
        }

        public VMFancyCollectionList(CollectionModel Model, bool CollapsedView = false) : base(Model)
        {
            _CollapsedView = CollapsedView;
            SyncWithModel();
            Model.CollectionItems.CollectionChanged += (o, e) => { SyncWithModel(); };
        }

        public void SyncWithModel()
        {
            if (!Application.Current.Dispatcher.CheckAccess())
            {
                Action aAction = new Action(() => { SyncWithModel(); });
                Application.Current.Dispatcher.BeginInvoke(aAction);
                return;
            }

            Cards.Clear();
            foreach (CardModel cm in Model.CollectionItems)
            {
                VFancyCollectionItem fancyCollectionItemV = new VFancyCollectionItem();
                VMFancyCollectionItem fancyCollectionItemVM = new VMFancyCollectionItem(cm);
                fancyCollectionItemV.DataContext = fancyCollectionItemVM;
                Cards.Add(fancyCollectionItemV);
            }
        }
    }
}
