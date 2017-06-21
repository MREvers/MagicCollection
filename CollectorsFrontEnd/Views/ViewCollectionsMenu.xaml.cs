using CollectorsFrontEnd.StoreFrontSupport;
using CollectorsFrontEnd.Views.ViewComponents;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;

namespace CollectorsFrontEnd.Views
{
    /// <summary>
    /// Interaction logic for ViewCollectionsMenu.xaml
    /// </summary>
    public partial class ViewCollectionsMenu : UserControl, INotifyPropertyChanged, IView
    {
        #region Bindings
        private MultiDisplay _OperationWindow = new MultiDisplay();
        public MultiDisplay OperationWindow
        {
            get
            {
                return _OperationWindow;
            }
            set
            {
                _OperationWindow = value;
                OnPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        #endregion

        #region Properties
        
        public VMCollectionsMenu VMCollectionsMenu { get; set; }

        #endregion

        #region Events
        public event DisplayEventHandler DisplayEvent;
        #endregion

        #region Public Methods
        public ViewCollectionsMenu()
        {
            InitializeComponent();
            VMCollectionsMenu = new VMCollectionsMenu(Model: this);
            DataContext = VMCollectionsMenu;

            OperationWindow.SetNewDisplay(
                    Name: "AvailableCollections",
                    NewDisplay: new VCCollectionsMenuList(),
                    DataContext: this.DataContext,
                    Persist: false);
            OperationWindow.DisplayEvent += eMultiDisplayEventHandler;

            // Call the server function which takes the callback and whatever args.
            // The server queues up that function and the call back.
            ServerInterface.Server.GetLoadedCollectionList(refreshCollectionListDisplay, UICallback: true);
        }

        public List<object> GetViewHook(ViewComponentHandles ViewHandle)
        {
            throw new NotImplementedException();
        }
        #endregion

        #region Private Methods

        /// <summary>
        /// Notifies the ViewModel to update the collection list display.
        /// </summary>
        /// <param name="alstCollList">New List of Loaded Collections</param>
        private void refreshCollectionListDisplay(IEnumerable<string> alstCollList)
        {
            VMCollectionsMenu.AvailableCollections.Clear();
            foreach(string szCollection in alstCollList)
            {
                VMCollectionsMenu.AvailableCollections.Add(szCollection);
            }
        }

        /// <summary>
        /// Notifies the ViewModel to update the collection preview display.
        /// </summary>
        /// <param name="alstPreviewList">Collection Preview</param>
        private void refreshCollectionPreview(IEnumerable<string> alstPreviewList)
        {
            VMCollectionsMenu.CollectionPreview.Clear();
            foreach (string szCollectionItem in alstPreviewList)
            {
                VMCollectionsMenu.CollectionPreview.Add(szCollectionItem);
            }
        }

        #endregion

        #region Event Handlers

        /// <summary>
        /// A function like this is needed whenever a MultiDisplay is used so that
        /// this class can handle events in a way that doesn't require the Display
        /// to know about this class.
        /// </summary>
        /// <param name="aszEvent"></param>
        private void eMultiDisplayEventHandler(object aoSource, DisplayEventArgs aoEvent)
        {
            if (aoEvent.Source == "VCCollectionsMenuList")
            {
                eCollectionsMenuListEventHandler(aoSource, aoEvent);
            }
        }

        /// <summary>
        /// An event handler like this will exist for each of the possible displays used.
        /// </summary>
        /// <param name="Property"></param>
        /// <param name="Event"></param>
        private void eCollectionsMenuListEventHandler(object aoSource, DisplayEventArgs aoEvent)
        {
            if (aoEvent.Property == "CollectionSelection")
            {
                if (aoEvent.Event == "SelectionChanged")
                {
                    eCollectionSelection_SelectionChanged();
                }
            }
            else if (aoEvent.Property == "ViewCollectionButton")
            {
                if (aoEvent.Event == "Clicked")
                {
                    aoEvent.Add("Selection", VMCollectionsMenu.SelectedCollection);
                    DisplayEvent(Source: aoSource, Event: aoEvent);
                }
            }
        }

        #endregion

        #region UI Event Handlers
        // These need not be called directly from the UI. If they are the end result of a
        // UI event, then it belongs here.

        private void eCollectionSelection_SelectionChanged()
        {
            string szSelectedCollection = this.VMCollectionsMenu.SelectedCollection;
            ServerInterface.Server.GetCollectionModel(
                CollectionName: szSelectedCollection,
                Callback: (aoCM) => { refreshCollectionPreview(aoCM.CollectionItems.Select(x => x.GetIdealIdentifier())); },
                UICallback: true);
        }

        #endregion
    }
}
