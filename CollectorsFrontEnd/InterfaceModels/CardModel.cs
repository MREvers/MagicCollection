using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace CollectorsFrontEnd.InterfaceModels
{
    public class CardModel : IDataModel, INotifyPropertyChanged
    {

        public string CardName { get; set; }
        public int Amount { get; set; }
        public string CardNameLong;
        public string TargetCollection;

        public List<Tuple<string, string>> LstMetaTags;
        public List<Tuple<string, string>> LstSpecifiedAttrs; // Attrs that can change between copy such as 'Set'
        public List<Tuple<string, string>> LstIdentifiedAttrs; // Attrs that define a copy into a card class.

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
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

        public CardModel(string aszCardName, string aszTargetCollection,
            List<Tuple<string, string>> aLstSpecifiedAttrs,
            List<Tuple<string, string>> aLstIdentifiedAttrs,
            List<Tuple<string, string>> aLstMetaTags)
        {
            CardName = aszCardName;
            TargetCollection = aszTargetCollection;
            LstMetaTags = aLstMetaTags;
            LstSpecifiedAttrs = aLstSpecifiedAttrs;
            LstIdentifiedAttrs = aLstIdentifiedAttrs;
            /*
            if (LstMetaTags.FirstOrDefault(x => x.Item1 == "Generalization") == null)
            {
                LstMetaTags.Add(new Tuple<string, string>("Generalization", "Main"));
            }
            */
        }

        public void SetAuxData(int aiAmount, string aszCardNameLong)
        {
            Amount = aiAmount;
            CardNameLong = aszCardNameLong;
        }

        public void AddMetaTag(Tuple<string,string> aTupKeyVal)
        {
            ServerInterfaceModel
                .CardClassInterfaceModel
                .AddMetaTag(
                TargetCollection,
                CardNameLong,
                aTupKeyVal.Item1,
                aTupKeyVal.Item2,
                LstMetaTags);
        }

        public void RemoveMetaTag(Tuple<string, string> aTupKeyVal)
        {
            ServerInterfaceModel
                .CardClassInterfaceModel
                .RemoveMetaTag(
                TargetCollection,
                CardNameLong,
                aTupKeyVal.Item1,
                LstMetaTags);
        }

        public string GetMetaTag(string aszKey)
        {
            string szRetVal = "";
            foreach (Tuple<string, string> KeyVal in LstMetaTags)
            {
                if (KeyVal.Item1 == aszKey)
                {
                    szRetVal = KeyVal.Item2;
                    break;
                }
            }
            return szRetVal;
        }

        // Does not count meta tags
        public bool IsSameAs(CardModel aoCardModel)
        {
            return ServerInterfaceModel.CardClassInterfaceModel.AreCardsSame(this, aoCardModel);
        }

        public bool IsSameMetaTags(List<Tuple<string, string>> alstTupOne, List<Tuple<string, string>> alstTupTwo)
        {
            return ServerInterfaceModel.CardClassInterfaceModel.AreMetaTagsSame(alstTupOne, alstTupTwo);
        }

        public void GetImage()
        {
            if (CardImage == null)
            {
                ServerInterfaceModel.CardClassInterfaceModel.DownloadAndCacheImage(ImageLoaded, this);
            }
            else
            {
                OnPropertyChanged("CardImage");
            }
        }

        private void ImageLoaded(object sender, EventArgs e)
        {
            CardImage = (BitmapImage)sender;
        }
    }
}
