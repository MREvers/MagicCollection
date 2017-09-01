using StoreFrontPro.Views;
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
   /// <summary>
   /// Maintains a memoized copy of common card attributes. i.e.
   /// Attributes that do not change between copies of the same
   /// card. 
   /// 
   /// Represents a single card or a group of identical cards 
   /// within a collection. Stores meta data, and specialized
   /// identifiers for it's class of card. 
   /// </summary>
   partial class CardModel: IModel
   {
      public static List<string> TEMP_LST_IMPORTANT_IDENTS = new List<string>() { "set" };

      public string _CardNameLong;
      public string CardNameLong
      {
         get
         {
            _CardNameLong = CardName;
            MetaTags.ForEach(x => { _CardNameLong += x.Item1 + "=\"" + x.Item2 + "\" "; });
            return _CardNameLong;
         }
      }

      public CardModelBase Prototype
      {
         get { return GetPrototype(CardName); }
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
      public List<Tuple<string, string>> MetaTags;

      // Attrs that define a copy into a card class.
      public List<Tuple<string, string>> IdentifyingAttributes; 

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

         foreach (Tuple<string, string> MTag in MetaTags)
         {
            szMetaList += MTag.Item1 + "=\"" + MTag.Item2 + "\" ";
         }

         foreach (Tuple<string, string> MTag in MetaTags)
         {
            szMetaList += MTag.Item1 + "=\"" + MTag.Item2 + "\" ";
         }
         szMetaList += "}";
         return CardNameLong + " : " + szMetaList;
      }

      public string GetIdealIdentifier()
      {
         List<string> LstImpList = TEMP_LST_IMPORTANT_IDENTS;
         if (LstImpList.Count == 0) { return ""; }

         string szSupport = "";

         for (int i = 0; i < LstImpList.Count; i++)
         {
            szSupport += LstImpList[i] + "=" + GetAttr(LstImpList[i]);
            szSupport += ", ";
         }

         // Remove the last comma.
         szSupport = szSupport.Substring(0, szSupport.Length - 2);

         // Trim the single Key if there is one
         szSupport = szSupport.Substring(1 + LstImpList[0].Length);

         szSupport = " [" + szSupport + "]";

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
         foreach (var tup in IdentifyingAttributes)
         {
            if (tup.Item1 == aszKey)
            {
               return tup.Item2;
            }
         }

         foreach (var attr in Prototype.CommonAttributes)
         {
            if (attr.Key == aszKey)
            {
               return attr.Value;
            }
         }

         return "";
      }

      public void GetImage(Action<BitmapImage> ImageReceiver)
      {
         if (!m_bCardImageIsLoading)
         {
            m_bCardImageIsLoading = true;
            Action<BitmapImage> actionWrapper = 
               (image) => { m_bCardImageIsLoading = false; ImageReceiver(image); };
            ServerInterface.Card.DownloadAndCacheImage(actionWrapper, this);
         }
      }

      private void parseIdentifier(string aszIdentifier)
      {
         List<string> lstIdentifierAndTags = aszIdentifier.Split(':').ToList();

         string szIdentifier;
         if (aszIdentifier.Trim().Substring(0, 1) == "x")
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

         string szMeta = "";
         if (lstIdentifierAndTags.Count > 1)
         {
            szMeta = lstIdentifierAndTags[1].Trim();
            MetaTags = ParseTagList(szMeta);
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
            IdentifyingAttributes = ParseTagList(szDetails);
         }
         else
         {
            IdentifyingAttributes = new List<Tuple<string, string>>();
         }
      }

      #region IModel
      public void Register(IViewModel item)
      {
         throw new NotImplementedException();
      }

      public void UnRegister(IViewModel item)
      {

      }

      public void NotifyViewModel()
      {
         throw new NotImplementedException();
      }

      public void EnableNotification(bool abNotify)
      {
         throw new NotImplementedException();
      }

      public void DisableNotification()
      {
         throw new NotImplementedException();
      }

      public void Sync(bool ASync = true)
      {
         throw new NotImplementedException();
      }
      #endregion
   }
}
