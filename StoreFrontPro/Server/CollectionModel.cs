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

namespace StoreFrontPro.Server
{
    public partial class CollectionModel
    {
        private bool _IsCollapsedCollection;
        public bool IsCollapsedCollection
        {
            get { return _IsCollapsedCollection; }
            set { m_bHardRebuild = (_IsCollapsedCollection != value); _IsCollapsedCollection = value; }
        }

        public string CollectionName;
        public string ID;
        public ObservableCollection<CardModel> CollectionItems;

        private bool m_bHardRebuild = false;

        public CollectionModel(string aszID)
        {
            ID = aszID;
            _IsCollapsedCollection = true;
            CollectionItems = new ObservableCollection<CardModel>();

            // Synchonously build this collection.
            Sync(Async: false);
        }

        public void CreateChildCollection(string aszNewName)
        {
            ServerInterface.Server.CreateCollection(aszNewName, ID);
        }

        public void Sync(bool Async = true, Action aCallback = null)
        {
            if (Async)
            {
                ServerInterface.Collection.GetCollectionMetaDataAS(
                    ID, analyzeMetaData, true);

                ServerInterface.Collection.GetCollectionListAS(
                    ID, IsCollapsedCollection,
                    (alstCol) => { setCollectionModels(alstCol, aCallback); },
                    true);
            }
            else
            {
                List<string> lstMDs   = ServerInterface.
                                        Collection.
                                        GetCollectionMetaData(ID);
                analyzeMetaData(lstMDs);

                List<string> lstItems = ServerInterface.
                                        Collection.
                                        GetCollectionList(ID, _IsCollapsedCollection);
                setCollectionModels(lstItems);
            }

        }

        public void SetBaselineHistory()
        {
            //ServerInterfaceModel.CollectionInterfaceModel.SetBaselineHistory(CollectionName);
        }

        public void SaveCollection()
        {
            ServerInterface.Collection.SaveCollectionAS(ID);
        }

        public void SubmitBulkEdits(List<string> alstEdits, Action aCallBack = null)
        {
            ServerInterface.Collection.LoadBulkChangesAS(
                this.ID, alstEdits,
                () => { Sync(true, aCallBack); }, false);
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

            DisableEvents(CollectionItems);

            List<CardModel> lstRemoves = new List<CardModel>();
            if (!m_bHardRebuild)
            {
                foreach (CardModel cm in CollectionItems)
                {
                    // Since count is not picked up in the hash, it must be checked for.
                    string szTargetHash = cm.GetMetaTag("__hash");
                    int iFound = lstNewHashes.IndexOf(szTargetHash);
                    int iCount;
                    if (iFound != -1 &&
                        int.TryParse(lstNewCounts[iFound], out iCount) &&
                        iCount == cm.Count)
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

                for (int i = 0; i < lstRemoves.Count; i++)
                {
                    if (i == lstRemoves.Count - 1 && lstNewHashes.Count == 0)
                    {
                        ServerInterface.Server.SyncServerTask(() => { EnableEvents(CollectionItems); });
                    }
                    CardModel removeCard = lstRemoves[i];
                    ServerInterface.Server.SyncServerTask(() => { CollectionItems.Remove(removeCard); });
                }
            }
            else
            {
                CollectionItems.Clear();
            }

            for (int i = 0; i < lstNewHashes.Count; i++)
            {
                if (i == lstNewHashes.Count - 1)
                {
                    ServerInterface.Server.SyncServerTask(() => { EnableEvents(CollectionItems); });
                }
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

        private void analyzeMetaData(List<string> alstMeta)
        {
            foreach (string szLine in alstMeta)
            {
                if (szLine.Contains(":"))
                {

                }
                else
                {
                    List<string> lstSplitLine = szLine.Split('=').ToList();
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
    }
}
