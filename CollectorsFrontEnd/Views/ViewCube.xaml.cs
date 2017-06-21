using CollectorsFrontEnd.InterfaceModels;
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

namespace CollectorsFrontEnd.Views
{
    /// <summary>
    /// Interaction logic for ViewCube.xaml
    /// </summary>
    public partial class ViewCube : UserControl, INotifyPropertyChanged
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

        public VMCube VMCube { get; set; }

        #endregion

        #region Public Methods

        public ViewCube(CollectionModel CollectionToView)
        {
            InitializeComponent();

            this.VMCube = new VMCube(Model: this);
            DataContext = this.VMCube;

            VCGroupListView groupListView = new VCGroupListView(CollectionToView);

            OperationWindow.SetNewDisplay(
                    Name: "AvailableCollections",
                    NewDisplay: groupListView,
                    DataContext: this.DataContext,
                    Persist: false);
            OperationWindow.DisplayEvent += eMultiDisplayEventHandler;

            groupListView.Update();
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

            if (aoEvent.Source == "VCGroupList")
            {
                //eCollectionsMenuListEventHandler(Property: szProperty, Event: szEvent, Source: aoSource);
            }
        }

        #endregion
    }
}
