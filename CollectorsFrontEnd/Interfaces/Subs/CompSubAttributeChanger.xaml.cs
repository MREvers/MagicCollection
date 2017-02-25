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

        public CardModel DataModel;

        public CompSubAttributeChanger(CardModel aDataModel)
        {
            InitializeComponent();
            DataContext = this;
            DataModel = aDataModel;
            DataModel.PropertyChanged += ImageLoaded;
            DataModel.GetImage();
        }

        private void ImageLoaded(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "CardImage")
            {
                CardModel dataModel = (CardModel)sender;
                CardImage = (BitmapImage)dataModel.CardImage;
            }
        }

        public event ComponentEvent UnhandledEvent;

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        public IDataModel GetDataModel()
        {
            return DataModel;
        }

        public void GetImage()
        {
            // At some point, add caching
            /*
            string szMUID = DataModel.GetMetaTag("multiverseid");
            BitmapImage bi3 = new BitmapImage();
            bi3.BeginInit();
            if (!string.IsNullOrEmpty(szMUID ))
            {
                bi3.UriSource =
                    new Uri(@"http://gatherer.wizards.com/Handlers/Image.ashx?multiverseid=" +
                    szMUID + @"&type=card", UriKind.RelativeOrAbsolute);
            }
            else
            {
                bi3.UriSource =
                    new Uri(@"http://gatherer.wizards.com/Handlers/Image.ashx?name=" +
                    DataModel.CardName + "&type=card", UriKind.RelativeOrAbsolute);
            }
            bi3.CacheOption = BitmapCacheOption.OnLoad;
            bi3.EndInit();
            CardImage = bi3;
            */
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
