using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace StoreFrontPro.Views.Components.CardImageDisplayer
{
    class VMCardImageDisplayer : ViewModel<CardModel>
    {
        private string _CardName;
        public string CardName
        {
            get { return _CardName; }
            set { _CardName = value; OnPropertyChanged(); }
        }

        private string _CardType;
        public string CardType
        {
            get { return _CardType; }
            set { _CardType = value; OnPropertyChanged(); }
        }

        private string _CardLoyalty;
        public string CardLoyalty
        {
            get { return _CardLoyalty; }
            set { _CardLoyalty = value; OnPropertyChanged(); }
        }

        private string _CardText;
        public string CardText
        {
            get { return _CardText; }
            set {_CardText = value; OnPropertyChanged(); }
        }

        private string _CardPT;
        public string CardPT
        {
            get { return _CardPT; }
            set { _CardPT = value; OnPropertyChanged(); }
        }

        private string _CardSet;
        public string CardSet
        {
            get { return _CardSet; }
            set { _CardSet = value; OnPropertyChanged(); }
        }

        private BitmapImage _CardImage = null;
        public BitmapImage CardImage
        {
            get { return _CardImage; }
            set { _CardImage = value; OnPropertyChanged(); }
        }

        public VMCardImageDisplayer(CardModel Model) : base(Model)
        {
            CardName = Model.CardName;
            CardType = Model.GetAttr("type");
            CardLoyalty = Model.GetAttr("loyalty");
            CardText = Model.GetAttr("text");
            CardPT = Model.GetAttr("power") + "/" + Model.GetAttr("toughness");
            CardSet = Model.GetAttr("set");

            //Model.GetImage(  );
        }
    }
}
