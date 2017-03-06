using CollectorsFrontEnd.Interfaces.Subs;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace CollectorsFrontEnd.InterfaceModels
{
    public class CollectionModel: IDataModel
    {
        public string CollectionName;
        public List<CardModel> LstCopyModels;

        public CollectionModel(string aszName, List<Tuple<string, List<Tuple<string, string>>>> aLstCards)
        {
            CollectionName = aszName;
            LstCopyModels = new List<CardModel>();
            BuildCopyModelList(aLstCards);

        }

        public void BuildCopyModelList(List<Tuple<string, List<Tuple<string, string>>>> aLstCards)
        {
            LstCopyModels.Clear();
            foreach (var LongNameTagsPair in aLstCards)
            {
                CardModel oCopy = ServerInterfaceModel.GenerateCopyModel(LongNameTagsPair.Item1, CollectionName, LongNameTagsPair.Item2);
                LstCopyModels.Add(oCopy);
            }
        }

        public void SaveCollection()
        {
            ServerInterfaceModel.CollectionInterfaceModel.SaveCollection(CollectionName);
        }

        public void AddItem(string aszCardNameLong, List<Tuple<string, string>> alstMeta)
        {
            ServerInterfaceModel.CollectionInterfaceModel.AddItem(CollectionName, aszCardNameLong, alstMeta);
        }

        public void RemoveItem(string aszCardNameLong, List<Tuple<string, string>> alstMeta)
        {
            ServerInterfaceModel.CollectionInterfaceModel.RemoveItem(CollectionName, aszCardNameLong, alstMeta);
        }

        public void Refresh()
        {
            ServerInterfaceModel.CollectionInterfaceModel.Refresh(CollectionName);
        }

    }
}
