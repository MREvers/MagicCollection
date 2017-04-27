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
    public partial class Module_CardDisplayer : UserControl, IComponent
    {
        public string CardName { get; set; }
        public string CardType { get; set; }
        public string CardLoyalty { get; set; }
        public string CardText { get; set; }
        public string CardPT { get; set; }
        public string CardSet { get; set; }
        public BitmapImage CardImage { get; set; } 

        public event ComponentEvent UnhandledEvent;

        public CardModel DataModel;

        public Module_CardDisplayer(CardModel aDataModel)
        {
            InitializeComponent();
            DataContext = this;
            DataModel = aDataModel;
            LoadNewSet(aDataModel);

            Unloaded += eUnload;
        }

        public void LoadNewSet(CardModel aDataModel)
        {
            if (aDataModel == null)
            {
                return;
            }

            DataModel.PropertyChanged -= eImageLoaded;
            DataModel.UnloadImage();
            CardImage = null;
            UpdateLayout();

            DataModel = aDataModel;
            CardName = DataModel.CardName;
            CardType = DataModel.GetAttr("type");
            CardLoyalty = DataModel.GetAttr("loyalty");
            CardText = DataModel.GetAttr("text");
            CardPT = DataModel.GetAttr("power") + "/" + DataModel.GetAttr("toughness");
            CardSet = DataModel.GetAttr("set");
            RefreshBindings();

            DataModel.PropertyChanged += eImageLoaded;
            DataModel.GetImage();
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
        private void eImageLoaded(object sender, PropertyChangedEventArgs e)
        {
            CardModel dataModel = (CardModel)sender;
            if (e.PropertyName == "CardImage" && dataModel.CardImage != null)
            {
                dataModel.CardImage.Freeze();
                if (CardImage != null)
                {
                    CardImage.Freeze();
                }
                CardImage = (BitmapImage)dataModel.CardImage.GetAsFrozen();
                this.Width = CardImage.Width;
                this.Height = CardImage.Height;
                RefreshBindings();
            }
        }

        private void RefreshBindings()
        {
            DataContext = null;
            DataContext = this;
        }

        private void eUnload(object sender, RoutedEventArgs e)
        {
            if (DataModel != null)
            {
                DataModel.PropertyChanged -= eImageLoaded;
                DataModel.UnloadImage();
            }
        }
    }
}
