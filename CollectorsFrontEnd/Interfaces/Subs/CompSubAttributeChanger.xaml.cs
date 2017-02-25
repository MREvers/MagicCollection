using CollectorsFrontEnd.InterfaceModels;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
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

namespace CollectorsFrontEnd.Interfaces.Subs
{
    /// <summary>
    /// Interaction logic for CompSubAttributeChanger.xaml
    /// </summary>
    public partial class CompSubAttributeChanger : UserControl, IComponent, INotifyPropertyChanged
    {
        #region Data Binding
        private BitmapImage _CardImage;
        public BitmapImage CardImage {
            get
            {
                return _CardImage;
            }
            set
            {
                _CardImage = value;
                OnPropertyChanged("CardImage");
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        #endregion

        public event ComponentEvent UnhandledEvent;

        public CardModel DataModel;

        public CompSubAttributeChanger(CardModel aDataModel)
        {
            InitializeComponent();
            DataContext = this;
            DataModel = aDataModel;
            DataModel.PropertyChanged += ImageLoaded;
            DataModel.GetImage();
        }

        public IDataModel GetDataModel()
        {
            return DataModel;
        }

        private void ImageLoaded(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "CardImage")
            {
                CardModel dataModel = (CardModel)sender;
                CardImage = (BitmapImage)dataModel.CardImage;
            }
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        private void eOK_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "AttrChanger.Close");
        }

        private void eCancel_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "AttrChanger.Close");
        }
    }
}
