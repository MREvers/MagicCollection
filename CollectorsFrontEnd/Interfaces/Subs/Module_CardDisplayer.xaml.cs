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
    /// Interaction logic for CompSubCardDisplayer.xaml
    /// </summary>
    public partial class Module_CardDisplayer : UserControl, IComponent, INotifyPropertyChanged
    {
        #region Data Binding
        private string _CardName;
        public string CardName
        {
            get
            {
                return _CardName;
            }
            set
            {
                _CardName = value;
                OnPropertyChanged("CardName");
            }
        }

        private string _CardType;
        public string CardType
        {
            get
            {
                return _CardType;
            }
            set
            {
                _CardType = value;
                OnPropertyChanged("CardType");
            }
        }

        private string _CardLoyalty;
        public string CardLoyalty
        {
            get
            {
                return _CardLoyalty;
            }
            set
            {
                _CardLoyalty = value;
                OnPropertyChanged("CardLoyalty");
            }
        }

        private string _CardText;
        public string CardText
        {
            get
            {
                return _CardText;
            }
            set
            {
                _CardText = value;
                OnPropertyChanged("CardText");
            }
        }

        private string _CardPT;
        public string CardPT
        {
            get
            {
                return _CardPT;
            }
            set
            {
                _CardPT = value;
                OnPropertyChanged("CardPT");
            }
        }

        private string _CardSet;
        public string CardSet
        {
            get
            {
                return _CardSet;
            }
            set
            {
                _CardSet = value;
                OnPropertyChanged("CardSet");
            }
        }

        private BitmapImage _CardImage;
        public BitmapImage CardImage
        {
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

        #region Public Events

        public event ComponentEvent UnhandledEvent;

        #endregion

        #region Public Feilds
        public CardModel DataModel;
        #endregion

        #region Public Functions
        public Module_CardDisplayer(CardModel aDataModel)
        {
            InitializeComponent();
            DataContext = this;
            DataModel = aDataModel;
            LoadNewSet(aDataModel);
            
        }

        public void LoadNewSet(CardModel aDataModel)
        {
            if (aDataModel == null)
            {
                return;
            }

            releaseBindings();

            CardName = aDataModel.CardName;
            CardType = aDataModel.GetAttr("type");
            CardLoyalty = aDataModel.GetAttr("loyalty");
            CardText = aDataModel.GetAttr("text");
            CardPT = aDataModel.GetAttr("power") + "/" + aDataModel.GetAttr("toughness");
            CardSet = aDataModel.GetAttr("set");

            aDataModel.PropertyChanged += eImageLoaded;
            aDataModel.GetImage();
        }

        public IDataModel GetDataModel(CardModel aDataModel)
        {
            throw new NotImplementedException();
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        public IDataModel GetDataModel()
        {
            throw new NotImplementedException();
        }

        #endregion

        #region Private Events

        private void refreshBindings()
        {
            DataContext = null;
            DataContext = this;
        }

        private void releaseBindings()
        {
            if (DataModel != null)
            {
                DataModel.PropertyChanged -= eImageLoaded;
                DataModel.UnloadImage();
            }
            CardImage = null;
            UpdateLayout();
        }
        #endregion

        #region UI Event Handlers
        private void eImageLoaded(object sender, PropertyChangedEventArgs e)
        {
            CardModel dataModel = (CardModel)sender;
            if (e.PropertyName == "CardImage" && dataModel.CardImage != null)
            {
                CardImage = null;
                UpdateLayout();
                DataModel = dataModel;
                dataModel.CardImage.Freeze();
                CardImage = dataModel.CardImage;
                Width = CardImage.Width;
                Height = CardImage.Height;
            }
        }

        private void eUnload(object sender, RoutedEventArgs e)
        {
            releaseBindings();
        }
        #endregion
    }
}
