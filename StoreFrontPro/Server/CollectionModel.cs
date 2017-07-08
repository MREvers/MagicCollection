using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using StoreFrontPro.Tools;

namespace StoreFrontPro.Server
{
    public class CollectionModel
    {
        private bool _IsCollapsedCollection = false;
        public bool IsCollapsedCollection
        {
            get { return _IsCollapsedCollection; }
            set { m_bHardRebuild = (_IsCollapsedCollection != value); _IsCollapsedCollection = value;  }
        }

        public string CollectionName;
        public ObservableCollection<CardModel> CollectionItems;
        public List<CardModel> LstLastQuery; // NOT CURRENTLY USED

        private bool m_bHardRebuild = false;

        public CollectionModel(string aszName)
        {
            CollectionName = aszName;
            CollectionItems = new ObservableCollection<CardModel>();
            LstLastQuery = new List<CardModel>();

            Sync();
        }

        public void Sync(Action aCallback = null)
        {
            ServerInterface.Collection.GetCollectionList(
                CollectionName,
                IsCollapsedCollection,
                (alstCol) => { setCollectionModels(alstCol, aCallback); },
                true);
        }

        public void SetBaselineHistory()
        {
            //ServerInterfaceModel.CollectionInterfaceModel.SetBaselineHistory(CollectionName);
        }

        public void SaveCollection()
        {
            ServerInterface.Collection.SaveCollection(CollectionName);
        }

        public void SubmitBulkEdits(List<string> alstEdits, Action aCallBack = null)
        {
            ServerInterface.Collection.LoadBulkChanges(
                this.CollectionName,
                alstEdits,
                () => { Sync(aCallBack); },
                false);
        }

        public void AddItem(string aszCardNameLong, List<Tuple<string, string>> alstMeta)
        {
            //ServerInterfaceModel.CollectionInterfaceModel.AddItem(CollectionName, aszCardNameLong, alstMeta);
        }

        public void RemoveItem(string aszCardNameLong, List<Tuple<string, string>> alstMeta)
        {
            //ServerInterfaceModel.CollectionInterfaceModel.RemoveItem(CollectionName, aszCardNameLong, alstMeta);
        }

        public void Refresh()
        {
            //ServerInterfaceModel.CollectionInterfaceModel.Refresh(CollectionName);
        }

        public List<string> GetAllCardsStartingWith(string aszString)
        {
            List<CardModel> LstStartingStrings = new List<CardModel>();
            List<CardModel> LstContainingStrings = new List<CardModel>();
            foreach (CardModel CM in CollectionItems)
            {
                string szShortName = CM.CardName.ToLower();
                if (szShortName.Contains(aszString))
                {
                    if (szShortName.Substring(0, aszString.Length) == aszString)
                    {
                        LstStartingStrings.Add(CM);
                    }
                    else
                    {
                        LstContainingStrings.Add(CM);
                    }
                }
            }

            LstLastQuery = LstStartingStrings.Concat(LstContainingStrings).ToList();

            return LstLastQuery.Select(x => x.CardNameLong).ToList();
        }

        private void setCollectionModels(List<string> aLstCards)
        {
            // Calculate differences.
            List<string> lstNewHashes = aLstCards.Select(x=>fastExtractHash(x)).ToList();
            if (!m_bHardRebuild)
            {
                List<CardModel> lstRemoves = new List<CardModel>();
                foreach (CardModel cm in CollectionItems)
                {
                    string szTargetHash = cm.GetMetaTag("__hash");
                    int iFound = lstNewHashes.IndexOf(szTargetHash);
                    if (iFound != -1)
                    {
                        // These CMs stay in the list
                        lstNewHashes.RemoveAt(iFound);
                        aLstCards.RemoveAt(iFound);
                    }
                    else
                    {
                        // These CMs are removed
                        lstRemoves.Add(cm);
                    }
                }

                foreach (CardModel removeCard in lstRemoves)
                {
                    ServerInterface.Server.SyncServerTask(() => { CollectionItems.Remove(removeCard); });
                }
            }
            else
            {
                CollectionItems.Clear();
            }

            for(int i = 0; i < lstNewHashes.Count; i++)
            {
                ServerInterface.Server.GenerateCopyModel(
                                    Identifier: aLstCards[i],
                                    CollectionName: CollectionName,
                                    Callback: (aoCardModel) => { CollectionItems.Add(aoCardModel); },
                                    UICallback: true);
            }

            m_bHardRebuild = false;
        }

        private void setCollectionModels(List<string> aLstCards, Action aCallback)
        {
            setCollectionModels(aLstCards);
            ServerInterface.Server.SyncServerTask(aCallback);
        }

        private string fastExtractHash(string aszIdentifier)
        {
            int iHashStart = aszIdentifier.IndexOf("__hash");
            if (!(iHashStart >= 0 && iHashStart < aszIdentifier.Length)) { return ""; }
            string remainingString = aszIdentifier.Substring(iHashStart);

            int iOpeningQuote = remainingString.IndexOf('\"');
            if (!(iOpeningQuote >= 0 && iOpeningQuote < aszIdentifier.Length)) { return ""; }

            int iClosingQuote = remainingString.IndexOf("\"", iOpeningQuote + 1);
            if (!(iClosingQuote >= 0 && iClosingQuote < aszIdentifier.Length)) { return ""; }

            return remainingString.Substring(iOpeningQuote + 1, iClosingQuote - iOpeningQuote - 1).Trim();

        }
    }
}
