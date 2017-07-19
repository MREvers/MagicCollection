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
        private static Dictionary<string, List<Tuple<string, string>>> MEMO_COMMON_ATTRS = new Dictionary<string, List<Tuple<string, string>>>();
        private static Dictionary<string, List<Tuple<string, List<string>>>> MEMO_ID_OPTIONS = new Dictionary<string, List<Tuple<string, List<string>>>>();
        public static List<Tuple<string, string>> GetCommonAttrs(string CardName)
        {
            if (!MEMO_COMMON_ATTRS.ContainsKey(CardName))
            {
                List<Tuple<string, string>> CommonAttributes = parseTagList(ServerInterface.Card.GetProtoType(CardName));
                if (CommonAttributes.Count > 0)
                {
                    MEMO_COMMON_ATTRS[CardName] = CommonAttributes;
                }

                List<Tuple<string, string>> lstNonComms = new List<Tuple<string, string>>();
                foreach (Tuple<string, string> comAttr in CommonAttributes)
                {
                    if (comAttr.Item2.Contains("*"))
                    {
                        string szVal = comAttr.Item2.Substring(1);
                        List<string> lstSplitOps = szVal.Split(new string[] { "::" }, StringSplitOptions.RemoveEmptyEntries).ToList();
                        lstNonComms.Add(comAttr);
                        Tuple<string, List<string>> tupOption = new Tuple<string, List<string>>(
                                comAttr.Item1,
                                lstSplitOps);
                        if (MEMO_ID_OPTIONS.ContainsKey(CardName))
                        {
                            MEMO_ID_OPTIONS[CardName].Add(tupOption);
                        }
                        else
                        {
                            MEMO_ID_OPTIONS[CardName] = new List<Tuple<string, List<string>>>() { tupOption };
                        }
                    }
                }

                foreach (var item in lstNonComms)
                {
                    CommonAttributes.Remove(item);
                }
            }

            return MEMO_COMMON_ATTRS.ContainsKey(CardName) ? MEMO_COMMON_ATTRS[CardName] : new List<Tuple<string, string>>();
        }
        public static List<Tuple<string, List<string>>> GetIdentifierOptions(string CardName)
        {
            if (!MEMO_ID_OPTIONS.ContainsKey(CardName))
            {
                GetCommonAttrs(CardName);
            }

            return MEMO_ID_OPTIONS.ContainsKey(CardName) ? MEMO_ID_OPTIONS[CardName] : new List<Tuple<string, List<string>>>();
        }

        private object m_oLock = new object();
        private bool _bCardImageIsLoading = false;
        private bool m_bCardImageIsLoading
        {
            get { lock (m_oLock) { return _bCardImageIsLoading; } }
            set { lock (m_oLock) { _bCardImageIsLoading = value; } }
        }

        public int Count = 1; // Used if the list in a collection is "Collapsed"
        public string TargetCollection;
        public string CardName;
        public string CardNameLong;
        public List<Tuple<string, string>> MetaTags;
        public List<Tuple<string, string>> IdentifyingAttributes; // Attrs that define a copy into a card class.
        public List<Tuple<string, string>> CommonAttributes; // Attrs that can change between copy such as 'Set'
        public List<Tuple<string, List<string>>> IdentifyingOptions = new List<Tuple<string, List<string>>>();

        public CardModel(string aszIdentifier, string aszParent)
        {
            TargetCollection = aszParent;
            parseIdentifier(aszIdentifier);
        }

        public void SubmitFeatureChangesToServer(
            List<Tuple<string, string>> alstNewMeta,
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
            foreach (var KeyVal in MetaTags)
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
            foreach(var tup in IdentifyingAttributes.Concat(CommonAttributes))
            {
                if (tup.Item1 == aszKey)
                {
                    szRetVal = tup.Item2;
                    break;
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

            string szIdentifier;
            if (aszIdentifier.Trim().Substring(0,1) == "x")
            {
                string szCount = lstIdentifierAndTags[0].Substring(1, 1);
                int.TryParse(szCount, out this.Count);

                szIdentifier = lstIdentifierAndTags[0].Trim().Substring(2).Trim();
            }
            else
            {
                this.Count = 1;
                szIdentifier = lstIdentifierAndTags[0].Trim();
            }
           
            CardNameLong = szIdentifier;
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

            // Now parse the common attributes and the restrictions.
            CommonAttributes = GetCommonAttrs(szName);
            IdentifyingOptions = GetIdentifierOptions(szName);
        }

        private static List<Tuple<string,string>> parseTagList(string szTags)
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
