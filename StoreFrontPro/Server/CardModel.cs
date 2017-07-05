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
    public class CardModel
    {
        public static List<string> TEMP_LST_IMPORTANT_IDENTS = new List<string>() { "set" };

        public string CardName { get; set; }
        public string CardNameLong {
            get
            {
                string szIDList = "{ ";
                foreach (Tuple<string, string> MTag in CommonAttributes.Concat(IdentifyingAttributes))
                {
                    szIDList += MTag.Item1 + "=\"" + MTag.Item2 + "\" ";
                }
                szIDList += "}";
                return CardName +" "+ szIDList;
            }
        }
        public string TargetCollection;

        public List<Tuple<string, string>> MetaTags;
        public List<Tuple<string, string>> CommonAttributes; // Attrs that can change between copy such as 'Set'
        public List<Tuple<string, string>> IdentifyingAttributes; // Attrs that define a copy into a card class.

        private object m_oLock = new object();
        private bool _bCardImageIsLoading = false;
        private bool m_bCardImageIsLoading
        {
            get { lock (m_oLock) { return _bCardImageIsLoading; } }
            set { lock (m_oLock) { _bCardImageIsLoading = value; } }
        }

        public CardModel(string aszIdentifier, string aszParent)
        {
            TargetCollection = aszParent;
            parseIdentifier(aszIdentifier);
        }

        public void SubmitFeatureChangesToServer(List<Tuple<string, string>> alstNewMeta,
            List<Tuple<string, string>> alstNewAttrs)
        {

        }

        public string GetFullIdentifier()
        {
            string szMetaList = "{ ";
            foreach(Tuple<string,string> MTag in MetaTags)
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
            foreach (Tuple<string, string> KeyVal in MetaTags)
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
            foreach(var tup in IdentifyingAttributes)
            {
                if (tup.Item1 == aszKey)
                {
                    szRetVal = tup.Item2;
                    break;
                }
            }

            if (szRetVal == "")
            {
                foreach (var tup in CommonAttributes)
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

        public void GetImage(Action<BitmapImage> aCallBack)
        {
            if (!m_bCardImageIsLoading)
            {
                m_bCardImageIsLoading = true;
                Action<BitmapImage> actionWrapper = (image) => { m_bCardImageIsLoading = false; aCallBack(image); };
                ServerInterface.Card.DownloadAndCacheImage(actionWrapper, this);
            }
        }

        private void parseIdentifier(string aszIdentifier)
        {
            List<string> lstIdentifierAndTags = aszIdentifier.Split(':').ToList();
            string szIdentifier = lstIdentifierAndTags[0].Trim();
            string szMeta = "";
            if (lstIdentifierAndTags.Count > 1)
            {
                szMeta = lstIdentifierAndTags[1].Trim();
                MetaTags = parseTagList(szMeta);
            }
            else
            {
                MetaTags = new List<Tuple<string, string>>();
            }

            List<string> lstNameAndIDs = szIdentifier.Split('{').ToList();
            string szName = lstNameAndIDs[0].Trim();
            CardName = szName;

            string szDetails = "";
            if (lstNameAndIDs.Count > 1)
            {
                szDetails = lstNameAndIDs[1].Trim();
                IdentifyingAttributes = parseTagList(szDetails);
            }
            else
            {
                IdentifyingAttributes = new List<Tuple<string, string>>();
            }

            CommonAttributes = parseTagList(ServerInterface.Card.GetProtoType(CardName));
        }

        private List<Tuple<string,string>> parseTagList(string szTags)
        {
            string szRetval = szTags.Trim(' ', '{', '}', ' ');
            List<string> lstTuples = szRetval.Split(' ').ToList();
            List<Tuple<string, string>> lstRealTuples = new List<Tuple<string, string>>();
            foreach(string tup in lstTuples)
            {
                List<string> lstKeyAndVal = tup.Split('=').ToList();
                if (lstKeyAndVal.Count == 2)
                {
                    string szKey = lstKeyAndVal[0].Trim(' ');
                    string szVal = lstKeyAndVal[1].Trim(' ', '\"');
                    lstRealTuples.Add(new Tuple<string, string>(szKey, szVal));
                }
            }

            return lstRealTuples;
        }
    }
}
