using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using StoreFrontPro.Tools;
using System.Reflection;
using System.Collections.Specialized;
using StoreFrontPro.Views;

namespace StoreFrontPro.Server
{
   class CollectionDelta
   {
      public string Command { get; private set; }
      public string DisplayString { get; private set; }
      public int MaxRemoveCount { get; private set; }
      public List<string> SelectionOptions { get; private set; }

      private CollectionDelta(string Command, string DisplayString, int MaxRemove, List<string> Options)
      {
         this.Command = Command;
         this.DisplayString = DisplayString;
         this.MaxRemoveCount = MaxRemove;
         this.SelectionOptions = Options;
      }

      public static CollectionDelta GetDelta(string AddCard, string RemoveCardIdealID, CollectionModel Collection)
      {
         string szDisplay, szCmdString, szAddCardProto;
         int iMaxDeltaCount = int.MaxValue;
         List<string> lstAdditionOptions = null;
         CardModel oRemoveCard = null;
         List<CardModel> lstModels = Collection.CollectionItems.Item;

         AddCard = AddCard ?? "";
         RemoveCardIdealID = RemoveCardIdealID ?? "";

         if( RemoveCardIdealID != "" )
         {
            oRemoveCard = Collection.GetCardModel(RemoveCardIdealID);
            if( oRemoveCard == null )
            {
               RemoveCardIdealID = "";
            }
            else
            {
               iMaxDeltaCount = 0;
               lstModels.ForEach(x => {
                  if (oRemoveCard.GetMetaTag("__hash") == x.GetMetaTag("__hash"))
                  {
                     iMaxDeltaCount += x.Count;
                  }
               });
            }

         }

         if( AddCard != "" )
         {
            szAddCardProto = ServerInterface.Card.GetProtoType(AddCard);
            if( szAddCardProto == "" )
            {
               AddCard = "";
            }
            else
            {
               CardModel.GetPrototype(AddCard).AttributeOptions.TryGetValue("set", out lstAdditionOptions);
            }
         }

         if( AddCard != "" && RemoveCardIdealID != "" )
         {
            szDisplay = "% " + oRemoveCard.GetIdealIdentifier();
            szDisplay += " -> " + AddCard;

            szCmdString = "% " + oRemoveCard.GetFullIdentifier();
            szCmdString += " -> " + AddCard;
         }
         else if( AddCard != "" )
         {
            szDisplay = "+ " + AddCard;
            szCmdString = "+ " + AddCard;
         }
         else if( RemoveCardIdealID != "" )
         {
            szDisplay = "- " + oRemoveCard.GetIdealIdentifier();
            szCmdString = "- " + oRemoveCard.GetFullIdentifier();
         }
         else
         {
            return null;
         }

         lstAdditionOptions = lstAdditionOptions ?? new List<string>() { "" };
         return new CollectionDelta(szCmdString, szDisplay, iMaxDeltaCount, lstAdditionOptions);
      }
   }

   partial class CollectionModel : IModel
   {
      public List<CardModel> Collection
      {
         get { return CollectionItems.Item; }
      }

      public string ID;
      public string CollectionName;
      public BasicModel<List<CardModel>> CollectionItems;

      private bool m_bHardRebuild = false;

      public CollectionModel(string aszID)
      {
         CollectionItems = new BasicModel<List<CardModel>>(new List<CardModel>(), Sync);
         ID = aszID;

         // Synchonously build this collection.
         CollectionItems.Sync(ASync: false);
      }

      public void CreateChildCollection(string aszNewName)
      {
         ServerInterface.Server.CreateCollection(aszNewName, ID);
      }

      public void SetBaselineHistory()
      {

      }

      public void SaveCollection()
      {
         ServerInterface.Collection.SaveCollectionAS(ID);
      }

      public void SubmitBulkEdits(List<string> alstEdits)
      {
         ServerInterface.Collection.LoadBulkChangesAS(
             this.ID, alstEdits, () => { Sync(false); }, false);
      }

      public List<string> SearchCollection(string aszSearch)
      {
          List<string> lstRetVal = new List<string>();
          List<string> lstHoldVals = new List<string>();
          foreach (string item in Collection.Select(x => x.GetIdealIdentifier()))
          {
             int iMatchIndex = item.ToLower().IndexOf(aszSearch);
             if (iMatchIndex == 0)
             {
                lstRetVal.Add(item);
             }
             else if (iMatchIndex > 0)
             {
                lstHoldVals.Add(item);
             }
          }
          
          lstRetVal = lstRetVal.Concat(lstHoldVals).ToList();
          return lstRetVal;
      }

      public CollectionDelta GetDeltaCommand(string AddCard = "", string RemoveIdealIdentifier = "")
      {
         return CollectionDelta.GetDelta(AddCard, RemoveIdealIdentifier, this);
      }

      public CardModel GetCardModel(string IdealIdentifier)
      {
         return Collection.Where(x => x.GetIdealIdentifier() == IdealIdentifier).FirstOrDefault();
      }

      private void setCollectionModels(List<string> aLstCards)
      {
         // Calculate differences.
         List<string> lstHashesAndCounts = aLstCards
             .Select(x => fastExtractHash(x, true)).ToList();

         List<string> lstNewHashes = lstHashesAndCounts
             .Select(x => x.Split(',')[1]).ToList();

         List<string> lstNewCounts = lstHashesAndCounts
             .Select(x => x.Split(',')[0] == "" ? (1).ToString() : x.Split(',')[0]).ToList();

         List<CardModel> lstRemoves = new List<CardModel>();
         if (!m_bHardRebuild)
         {
            foreach (CardModel cm in CollectionItems.Item)
            {
               // Since count is not picked up in the hash, it must be checked for.
               int iCount;
               string szTargetHash = cm.GetMetaTag("__hash");
               int iFound = lstNewHashes.IndexOf(szTargetHash);

               if( ( iFound != -1 ) &&
                   ( int.TryParse(lstNewCounts[iFound], out iCount) ) &&
                   ( iCount == cm.Count ) )
               {
                  // These CMs stay in the list
                  lstNewCounts.RemoveAt(iFound);
                  lstNewHashes.RemoveAt(iFound);
                  aLstCards.RemoveAt(iFound);
               }
               else
               {
                  // These CMs are removed
                  lstRemoves.Add(cm);
               }
            }
         }
         else
         {
            CollectionItems.Item.Clear();
         }

         // Remove the removed cards.
         ServerInterface.Server.SyncServerTask(() =>
         {
            lstRemoves.ForEach(x => CollectionItems.Item.Remove(x));
         });

         // Add new cards.
         ServerInterface.Server.GenerateCopyModelsAS(
            Identifiers: aLstCards,
            CollectionName: CollectionName,
            Callback: (lst) => { lst.ForEach(x => { CollectionItems.Item.Add(x); }); },
            UICallback: true);

         ServerInterface.Server.SyncServerTask(CollectionItems.NotifyViewModel);
         m_bHardRebuild = false;
      }

      private void analyzeMetaData(List<string> alstMeta)
      {
         var itemTags = new List<string>();
         var collectionTags = new List<string>();
         foreach (string szLine in alstMeta)
         {
            if (szLine.Contains(":"))
            {
               itemTags.Add(szLine);
            }
            else
            {
               collectionTags.Add(szLine);
            }
         }

         foreach(var itemTag in itemTags)
         {
            // TODO: Handle These.
         }

         foreach(var collectionTag in collectionTags)
         {
            List<string> lstSplitLine = collectionTag.Split('=').ToList();
            if (lstSplitLine.Count > 1)
            {
               string szKey = lstSplitLine[0];
               string szVal = lstSplitLine[1];
               if (szKey == "Name")
               {
                  CollectionName = szVal.Trim('"');
               }
            }
         }
      }

      /// <summary>
      /// WithCount is included be a count change will not be detected by the hash.
      /// If withcount is true, there is guaranteed to be a comma in the return.
      /// </summary>
      /// <param name="aszIdentifier"></param>
      /// <param name="WithCount"></param>
      /// <returns></returns>
      private string fastExtractHash(string aszIdentifier, bool WithCount = false)
      {
         string szWithCount = "";
         int iFirstSpace = aszIdentifier.IndexOf(' ');
         if (WithCount)
         {
            szWithCount = aszIdentifier.Substring(0, iFirstSpace);
            if (szWithCount[0] == 'x')
            {
               szWithCount = szWithCount.Substring(1, iFirstSpace - 1);
            }
            szWithCount += ",";
         }

         int iHashStart = aszIdentifier.IndexOf("__hash");
         if (!(iHashStart >= 0 && iHashStart < aszIdentifier.Length)) { return ""; }
         string remainingString = aszIdentifier.Substring(iHashStart);

         int iOpeningQuote = remainingString.IndexOf('\"');
         if (!(iOpeningQuote >= 0 && iOpeningQuote < aszIdentifier.Length)) { return ""; }

         int iClosingQuote = remainingString.IndexOf("\"", iOpeningQuote + 1);
         if (!(iClosingQuote >= 0 && iClosingQuote < aszIdentifier.Length)) { return ""; }

         return szWithCount + remainingString.Substring(iOpeningQuote + 1, iClosingQuote - iOpeningQuote - 1).Trim();
      }

      #region IModel
      private List<IViewModel> m_lstViewers = new List<IViewModel>();
      public void Register(IViewModel item)
      {
         m_lstViewers.Add(item);
      }

      public void UnRegister(IViewModel item)
      {
         m_lstViewers.Remove(item);
      }

      public void NotifyViewModel()
      {
         m_lstViewers.ForEach(x => x.ModelUpdated());
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
         if (ASync)
         {
            ServerInterface.Server.SyncServerTask(()=> { Sync(false); });
         }
         else
         {
            List<string> lstMDs = ServerInterface.Collection.GetCollectionMetaData(ID);
            analyzeMetaData(lstMDs);

            List<string> lstItems = ServerInterface.Collection.GetCollectionList(ID);
            setCollectionModels(lstItems);

            NotifyViewModel();
         }
      }
      #endregion
   }
}
