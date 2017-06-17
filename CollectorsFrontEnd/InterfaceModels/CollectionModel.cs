using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace CollectorsFrontEnd.InterfaceModels
{
    public class CollectionModel
    {
        public string CollectionName;
        public List<CardModel> LstCopyModels;
        public List<CardModel> LstLastQuery;

        public CollectionModel(string aszName, List<Tuple<string, List<Tuple<string, string>>>> aLstCards)
        {
            CollectionName = aszName;
            LstCopyModels = new List<CardModel>();
            LstLastQuery = new List<CardModel>();
            BuildCopyModelList(aLstCards);

        }

        public void BuildCopyModelList(List<Tuple<string, List<Tuple<string, string>>>> aLstCards)
        {
            LstCopyModels.Clear();
            foreach (var LongNameTagsPair in aLstCards)
            {
                //CardModel oCopy = ServerInterfaceModel.GenerateCopyModel(LongNameTagsPair.Item1, CollectionName, LongNameTagsPair.Item2);
                //LstCopyModels.Add(oCopy);
            }
        }

        public void SetBaselineHistory()
        {
            //ServerInterfaceModel.CollectionInterfaceModel.SetBaselineHistory(CollectionName);
        }

        public void SaveCollection()
        {
            //ServerInterfaceModel.CollectionInterfaceModel.SaveCollection(CollectionName);
        }

        public void SubmitBulkEdits(List<string> alstEdits)
        {
            //ServerInterfaceModel.CollectionInterfaceModel.LoadBulkChanges(this.CollectionName, alstEdits);
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
            foreach (CardModel CM in LstCopyModels)
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

    }
}
