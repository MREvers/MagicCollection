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
            CardName = aDataModel.CardName;
            CardType = aDataModel.GetAttr("type");
            CardLoyalty = aDataModel.GetAttr("loyalty");
            CardText = aDataModel.GetAttr("text");
            CardPT = aDataModel.GetAttr("power") + "/" + aDataModel.GetAttr("toughness");
            CardSet = aDataModel.GetAttr("set");

            aDataModel.PropertyChanged += eImageLoaded;
            aDataModel.GetImage();
        }

        private void eImageLoaded(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "CardImage")
            {
                CardModel dataModel = (CardModel)sender;
                CardImage = (BitmapImage)dataModel.CardImage;
                this.Width = CardImage.Width;
                this.Height = CardImage.Height;
            }
        }

        public IDataModel GetDataModel()
        {
            throw new NotImplementedException();
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }
    }
}
