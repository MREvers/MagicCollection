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

      private string _CardNameLong;
      public string CardNameLong
      {
         get
         {
            _CardNameLong = PrototypeName;
            _CardNameLong += " { ";
            IdentifyingAttributes.ForEach(x => { _CardNameLong += x.Item1 + "=\"" + x.Item2 + "\" "; });
            _CardNameLong += "}";
            return _CardNameLong;
         }
      }

      public string DisplayName { get; private set; }
      public string PrototypeName { get; private set; }

      public CardModelBase Prototype
      {
         get { return GetPrototype(PrototypeName); }
      }

      public int Count = 1; 
      public string TargetCollection;
      public List<string> UIDs;

      // Attrs that define a copy into a card class.
      public List<Tuple<string, string>> IdentifyingAttributes; 

      /// <summary>
      /// Expects the identifier to be of the form
      /// CardNameLong : uidlist
      /// </summary>
      /// <param name="aszIdentifier"></param>
      /// <param name="aszParent"></param>
      public CardModel(string aszIdentifier, string aszParent)
      {
         TargetCollection = aszParent;
         parseIdentifier(aszIdentifier);
      }

      public void SetAttr(string aszKey, string aszVal, string aszUID = "")
      {
         string szUID = UIDOrDefault(aszUID);

         if( szUID != "" )
         {
            ServerInterface.Card.SetAttribute(PrototypeName, aszUID, aszKey, aszVal);
         }
      }

      public string GetFullIdentifier(string aszUID = "")
      {
         string szUID = UIDOrDefault(aszUID);
         var MetaTags = ServerInterface.Card.GetMetaTags(PrototypeName, szUID);

         string szMetaList = "{ ";

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

         return PrototypeName + szSupport;
      }

      /// <summary>
      /// Returns a metatag with the key first, then if there
      /// is an attr, but no meta, then return the attr.
      /// </summary>
      /// <param name="aszKey"></param>
      /// <returns></returns>
      public string GetFeature(string aszKey, string aszUID = "")
      {
         string szUID = UIDOrDefault(aszUID);
         string szRetval = GetMetaTag(aszKey, szUID);
         return szRetval == "" ? GetAttr(aszKey) : szRetval;
      }

      public string GetMetaTag(string aszKey, string aszUID = "")
      {
         string szUID = UIDOrDefault(aszUID);
         if( szUID == "" ) { return ""; }
         var MetaTags = ServerInterface.Card.GetMetaTags(PrototypeName, szUID);

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

      public string GetImagePath()
      {
         return Path.Combine( ServerInterface.Server.GetImagesFolderPath(),
                              "_" + GetAttr("set"),
                              getImageFileName() );
      }

      public void GetImage(Action<BitmapImage> ImageReceiver)
      {
         ServerInterface.Card.DownloadAndCacheImage(ImageReceiver, this);
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

         string szUIDs = "";
         List<Tuple<string,string>> lstUIDs = new List<Tuple<string, string>>();
         if (lstIdentifierAndTags.Count > 1)
         {
            szUIDs = lstIdentifierAndTags[1].Trim();
            lstUIDs = ParseTagList(szUIDs);
         }

         // Store the UIDs.
         this.UIDs = lstUIDs.Select(x => x.Item2).ToList();
      }

      private string UIDOrDefault(string aszUID)
      {
         string szUID = aszUID;
         if( szUID == "" && UIDs.Count > 0)
         {
            szUID = UIDs[0];
         }

         return szUID;
      }

      /*
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
         PrototypeName = szName;

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

         setDisplayName();
      }
      */

      private void setDisplayName()
      {
         DisplayName = PrototypeName;

         string szLayout = GetAttr("layout");
         string szNames = GetAttr("names");
         if(szLayout != "" && szLayout != "normal" && szLayout != "double-faced")
         {
            string[] lstNames = szNames.Split(new string[]{ "::" }, 2, StringSplitOptions.RemoveEmptyEntries);
            DisplayName = string.Join(" // ", lstNames);
         }
      }

      private string getImageFileName()
      {
         return PrototypeName + ".jpg";
      }

      #region IModel
      private bool m_bNotify = true;
      private List<WeakReference<IViewModel>> viewModels = new List<WeakReference<IViewModel>>();
      public void NotifyViewModel()
      {
         if( !m_bNotify ) { return; }
         viewModels.ForEach(x => 
         {
            IViewModel model;
            if(x.TryGetTarget(out model))
            {
               model.ModelUpdated();
            }
         });
      }

      public void Register(IViewModel item)
      {
         viewModels.Add(new WeakReference<IViewModel>(item));
      }

      public void UnRegister(IViewModel item)
      {
         // Find the model that corresponds.
         var lstRemoves = new List<WeakReference<IViewModel>>();
         foreach(var model in viewModels)
         {
            IViewModel test;
            if(model.TryGetTarget(out test))
            {
               if( test == item )
               {
                  lstRemoves.Add(model);
               }
            }
         }

         foreach( var model in lstRemoves )
         {
            viewModels.Remove(model);
         }
      }

      public void Sync(bool ASync = true)
      {
         //syncAction?.Invoke(ASync);
         //if (m_bNotify) { NotifyViewModel(); }
      }

      public void EnableNotification(bool abNotify = false)
      {
        // m_bNotify = true;
         //if (abNotify) { NotifyViewModel(); }
      }

      public void DisableNotification()
      {
       //  m_bNotify = false;
      }
      #endregion
   }
}
