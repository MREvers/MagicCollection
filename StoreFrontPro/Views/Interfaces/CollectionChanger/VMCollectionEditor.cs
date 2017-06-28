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
            ssBoxVMA.DisplayEvent += eAddCardEventHandler;

            MSuggestionsSearchBox ssBoxMR = new MSuggestionsSearchBox(
                ActionName: "Remove/Replace Card",
                Collection: Model.CollectionItems.Select(x => x.CardName));
            VMSuggestionsSearchBox ssBoxVMR = new VMSuggestionsSearchBox(ssBoxMR);
            RemoveCardSearchBox = new VSuggestionsSearchBox() { DataContext = ssBoxVMR };
            ssBoxVMR.DisplayEvent += eRemoveCardEventHandler;
        }

        private void eRemoveCardEventHandler(object Source, DisplayEventArgs Event)
        {
            // Only one event comes from the SSBox so we dont have to worry about checking.
        }

        private void eAddCardEventHandler(object Source, DisplayEventArgs Event)
        {
            throw new NotImplementedException();
        }

        private void eAcceptCommand(object canExecute)
        {

        }

        private void eCancelCommand(object canExecute)
        {

        }

    }
}
