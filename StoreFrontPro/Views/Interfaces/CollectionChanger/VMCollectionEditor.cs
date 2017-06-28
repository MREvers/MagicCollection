using StoreFrontPro.Server;
using StoreFrontPro.Views.Components.SuggestionsSearchBox;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Interfaces.CollectionChanger
{
    class VMCollectionEditor : ViewModel<CollectionModel>
    {
        public ObservableCollection<string> TextChangesList { get; set; } = new ObservableCollection<string>();

        public VSuggestionsSearchBox AddCardSearchBox { get; set; }
        public VSuggestionsSearchBox RemoveCardSearchBox { get; set; }

        public RelayCommand AcceptCommand { get; set; }
        public RelayCommand CancelCommand { get; set; }

        private List<string> m_lstRealChanges { get; set; }

        public VMCollectionEditor(CollectionModel Model) : base(Model)
        {
            AcceptCommand = new RelayCommand(eAcceptCommand);
            CancelCommand = new RelayCommand(eCancelCommand);
            m_lstRealChanges = new List<string>();

            MSuggestionsSearchBox ssBoxMA = new MSuggestionsSearchBox(
                ActionName: "Add Card",
                SearchCollection: (szSearch) => { return ServerInterface.Server.GetAllCardsStartingWith(szSearch); });
            VMSuggestionsSearchBox ssBoxVMA = new VMSuggestionsSearchBox(ssBoxMA);
            AddCardSearchBox = new VSuggestionsSearchBox() { DataContext = ssBoxVMA };

            MSuggestionsSearchBox ssBoxMR = new MSuggestionsSearchBox(
                ActionName: "Remove/Replace Card",
                SearchCollection: Model.CollectionItems.Select(x => x.CardName).ToList());

            VMSuggestionsSearchBox ssBoxVMR = new VMSuggestionsSearchBox(ssBoxMR);
            RemoveCardSearchBox = new VSuggestionsSearchBox() { DataContext = ssBoxVMR };
        }
        private void eAcceptCommand(object canExecute)
        {

        }

        private void eCancelCommand(object canExecute)
        {

        }

    }
}
