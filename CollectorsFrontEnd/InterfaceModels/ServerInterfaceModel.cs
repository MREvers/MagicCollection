using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CollectorsFrontEnd.InterfaceModels
{
    public static class ServerInterfaceModel
    {
        private static ServerClientInterface SCI = new ServerClientInterface();
        private static List<CollectionModel> LstCollectionModels = new List<CollectionModel>();

        #region Friend Interfaces
        public static class CollectionInterfaceModel
        {
            public static void SaveCollection(string aszCollectionName)
            {
                SCI.SaveCollection(aszCollectionName);
            }

            public static void AddItem(string aszCollectionName, string aszCardNameLong, List<Tuple<string,string>> lstMeta)
            {
                SCI.AddItem(aszCollectionName, aszCardNameLong, lstMeta);
            }

            public static void RemoveItem(string aszCollectionName, string aszCardNameLong, List<Tuple<string, string>> lstMeta)
            {
                SCI.RemoveItem(aszCollectionName, aszCardNameLong, lstMeta);
            }

            /// <summary>
            /// Calls the server for the most up to date list of copies.
            /// </summary>
            /// <param name="aszCollectionName"></param>
            public static void Refresh(string aszCollectionName)
            {
                if (GetCollectionModel(aszCollectionName) != null)
                {
                    // List of [ { CardNameLong, [Tags, ...] }, ... ]
                    List<Tuple<string, List<Tuple<string, string>>>> lstCards =
                        SCI.GetCollectionListWithMeta(aszCollectionName);
                    CollectionModel CMCurrent = LstCollectionModels.FirstOrDefault(x => x.CollectionName == aszCollectionName);
                    if (CMCurrent != null)
                    {
                        CMCurrent.BuildCopyModelList(lstCards);
                    }
                }
            }
        }

        public static class CardClassInterfaceModel
        {
            // Compares long name
            public static bool AreCardsSame(CardModel aoCardOne, CardModel aoCardTwo)
            {
                return SCI.IsSameIdentity(aoCardOne.CardNameLong, aoCardTwo.CardNameLong);
            }
        }
        #endregion

        /// <summary>
        /// Creates a new collection model from data from the server, if that collection model
        ///  doesn't already exist. If it exists, it updates the model.
        /// </summary>
        /// <param name="aszCollectionFileName"></param>
        /// <returns></returns>
        public static CollectionModel GenerateCollectionModel(string aszCollectionFileName)
        {
            string szColName = SCI.LoadCollection(aszCollectionFileName);
            // List of [ { CardNameLong, [Tags, ...] }, ... ]
            List<Tuple<string, List<Tuple<string, string>>>> lstCards =
                SCI.GetCollectionListWithMeta(szColName);
            CollectionModel CMNew = new CollectionModel(szColName, lstCards);
            CollectionModel CMCurrent = LstCollectionModels.FirstOrDefault(x => x.CollectionName == szColName);
            if (CMCurrent == null)
            {
                LstCollectionModels.Add(CMNew);
            }
            else
            {
                CMCurrent.LstCopyModels = CMNew.LstCopyModels;
            }
            
            return CMNew;
        }

        public static CardModel GenerateCopyModel(string aszCardNameLong, List<Tuple<string, string>> aLstMetaTags)
        {
            // Really, this SCI function just parses the long name.
            MCopyObject oParsed = ServerInterfaceModel.SCI.ConvertItemToCopyObject(aszCardNameLong);
            List<Tuple<string, string>> lstIdentifiedAttrs =
                ServerInterfaceModel.SCI.GetCardAttributes(aszCardNameLong);
            CardModel CopyM = new CardModel(
                oParsed.Name,
                oParsed.Attributes.Select(x => new Tuple<string, string>(x.Key,x.Value)).ToList(),
                lstIdentifiedAttrs,
                aLstMetaTags);
            CopyM.SetAuxData(oParsed.Amount, aszCardNameLong);

            return CopyM;
        }

        /// <summary>
        /// Returns the collection model if it exists. Null otherwise.
        /// </summary>
        /// <param name="aszCollectionName"></param>
        /// <returns></returns>
        public static CollectionModel GetCollectionModel(string aszCollectionName)
        {
            return LstCollectionModels.FirstOrDefault(x => x.CollectionName == aszCollectionName);
        }

        public static List<string> GetLoadedCollectionList()
        {
            return LstCollectionModels.Select(x => x.CollectionName).ToList();
        }

        public static List<string> GetAllCardsStartingWith(string aszStart)
        {
            return SCI.GetAllCardsStartingWith(aszStart);
        }
    }
}
