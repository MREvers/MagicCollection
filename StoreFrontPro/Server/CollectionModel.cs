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
        public bool IsCollapsedCollection = true;
        public string CollectionName;
        public ObservableCollection<CardModel> CollectionItems;
        public List<CardModel> LstLastQuery; // NOT CURRENTLY USED

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
                (alstCol)=> { setCollectionModels(alstCol, aCallback); },
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
            CollectionItems.Clear();
            foreach (var LongNameTagsPair in aLstCards)
            {
                ServerInterface.Server.GenerateCopyModel(
                    Identifier: LongNameTagsPair,
                    CollectionName: CollectionName,
                    Callback: (aoCardModel) => { CollectionItems.Add(aoCardModel); },
                    UICallback: true);
            }
        }

        private void setCollectionModels(List<string> aLstCards, Action aCallback)
        {
            setCollectionModels(aLstCards);
            ServerInterface.Server.SyncServerTask(aCallback);
        }


    }
}
