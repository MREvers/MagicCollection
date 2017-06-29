using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace StoreFrontPro.Server
{
    public class CardModel : INotifyPropertyChanged
    {
        public static List<string> TEMP_LST_IMPORTANT_IDENTS = new List<string>() { "set" };

        public string CardName { get; set; }
        public int Amount { get; set; }
        public string CardNameLong;
        public string TargetCollection;

        public List<Tuple<string, string>> MetaTagList;
        public List<Tuple<string, string>> SpecifiedAttrList; // Attrs that can change between copy such as 'Set'
        public List<Tuple<string, string>> IdentifiedAttrList; // Attrs that define a copy into a card class.
        public List<Tuple<string, List<string>>> SpecifiedAttrsRestrictionList;

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
            MetaTagList = aLstMetaTags;
            SpecifiedAttrList = aLstSpecifiedAttrs;
            IdentifiedAttrList = aLstIdentifiedAttrs;
        }

        public void SetAuxData(int aiAmount, string aszCardNameLong)
        {
            Amount = aiAmount;
            CardNameLong = aszCardNameLong;
            SpecifiedAttrsRestrictionList = new List<Tuple<string, List<string>>>();
            foreach (Tuple<string, string> TupKeyVal in SpecifiedAttrList)
            {
                List<string> lstRestrictions = getCardAttributeRestrictions(TupKeyVal.Item1);
                if (lstRestrictions[0] != "*")
                {
                    SpecifiedAttrsRestrictionList.Add(new Tuple<string, List<string>>(TupKeyVal.Item1, lstRestrictions));
                }
            }
        }

        public void AddMetaTag(Tuple<string,string> aTupKeyVal)
        {
            /*
            ServerInterfaceModel
                .CardClassInterfaceModel
                .AddMetaTag(
                TargetCollection,
                CardNameLong,
                aTupKeyVal.Item1,
                aTupKeyVal.Item2,
                LstMetaTags);
                */
        }

        public void RemoveMetaTag(string aszKey)
        {

        }

        public void SubmitMetaTagChangesToServer(List<Tuple<string, string>> alstNewMeta)
        {
 
        }

        public void SubmitFeatureChangesToServer(List<Tuple<string, string>> alstNewMeta,
            List<Tuple<string, string>> alstNewAttrs)
        {

        }

        public string GetFullIdentifier()
        {
            string szMetaList = "{ ";
            foreach(Tuple<string,string> MTag in MetaTagList)
            {
                szMetaList += MTag.Item1 + "=\"" + MTag.Item2 + "\" ";
            }
            szMetaList += "}";
            return CardNameLong + " : " + szMetaList;
        }

        public string GetIdealIdentifier()
        {
            List<string> LstImpList = TEMP_LST_IMPORTANT_IDENTS;
            string szSupport = " [";
            if (LstImpList.Count == 1)
            {
                szSupport += GetAttr(LstImpList[0])+"]";
            }
            else
            {
                for (int i = 0; i < LstImpList.Count; i++)
                {
                    szSupport += LstImpList[i] + "=" + GetAttr(LstImpList[i]);
                    if (i != LstImpList.Count - 1)
                    {
                        szSupport += ", ";
                    }
                    else
                    {
                        szSupport += "]";
                    }
                }

            }
            return CardName + szSupport;
        }

        public string GetMetaTag(string aszKey)
        {
            string szRetVal = "";
            foreach (Tuple<string, string> KeyVal in MetaTagList)
            {
                if (KeyVal.Item1 == aszKey)
                {
                    szRetVal = KeyVal.Item2;
                    break;
                }
            }
            return szRetVal;
        }

        public string GetAttr(string aszKey)
        {
            string szRetVal = "";
            foreach(var tup in IdentifiedAttrList)
            {
                if (tup.Item1 == aszKey)
                {
                    szRetVal = tup.Item2;
                    break;
                }
            }

            if (szRetVal == "")
            {
                foreach (var tup in SpecifiedAttrList)
                {
                    if (tup.Item1 == aszKey)
                    {
                        szRetVal = tup.Item2;
                        break;
                    }
                }
            }

            return szRetVal;
        }

        // Does not count meta tags
        public bool IsSameAs(CardModel aoCardModel)
        {
            return false;// ServerInterfaceModel.CardClassInterfaceModel.AreCardsSame(this, aoCardModel);
        }

        public bool IsSameMetaTags(List<Tuple<string, string>> alstTupOne, List<Tuple<string, string>> alstTupTwo)
        {
            return false;// ServerInterfaceModel.CardClassInterfaceModel.AreMetaTagsSame(alstTupOne, alstTupTwo);
        }

        public void GetImage()
        {
            if (CardImage == null)
            {
                //ServerInterface.Card.DownloadAndCacheImage()
             //   ServerInterfaceModel.CardClassInterfaceModel.DownloadAndCacheImage(ImageLoaded, this);
            }
            else
            {
                OnPropertyChanged("CardImage");
            }
        }

        public void UnloadImage()
        {
            if (_CardImage != null)
            {
                if (_CardImage.StreamSource != null)
                {
                    _CardImage.StreamSource.Close();
                    _CardImage.StreamSource.Dispose();
                }
                _CardImage = null;
            }
        }

        private void ImageLoaded(object sender, EventArgs e)
        {
            CardImage = (BitmapImage)sender;
        }

        private List<string> getCardAttributeRestrictions(string aszKey)
        {
            return ServerInterface.Card.GetCardAttributesRestrictions(CardNameLong, aszKey);
        }
    }
}
