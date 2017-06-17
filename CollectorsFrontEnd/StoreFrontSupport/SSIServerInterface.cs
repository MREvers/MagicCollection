using CollectorsFrontEnd.InterfaceModels;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CollectorsFrontEnd.StoreFrontSupport
{
    /// <summary>
    /// File Name explanation.
    /// S => Static
    /// SI => Server Interface
    /// </summary>
    public static partial class ServerInterface
    {
        private static ServerClientInterface SCI = ServerInterfaceModel.SCI;
        private static List<CollectionModel> ms_lstCollectionModels = new List<CollectionModel>();

        public static class Server
        {
            /// <summary>
            /// Creates a new collection model from data from the server, if that collection model
            ///  doesn't already exist. If it exists, it updates the model.
            /// </summary>
            /// <param name="aszCollectionFileName"></param>
            /// <returns></returns>
            public static CollectionModel GenerateCollectionModel(string aszCollectionFileName)
            {
                string szColName = SCI.LoadCollection(aszCollectionFileName);
                if (szColName != "")
                {
                    // List of [ { CardNameLong, [Tags, ...] }, ... ]
                    List<Tuple<string, List<Tuple<string, string>>>> lstCards =
                        SCI.GetCollectionListWithMeta(szColName);
                    CollectionModel CMNew = new CollectionModel(szColName, lstCards);
                    CollectionModel CMCurrent = ms_lstCollectionModels.FirstOrDefault(x => x.CollectionName == szColName);
                    if (CMCurrent == null)
                    {
                        ms_lstCollectionModels.Add(CMNew);
                    }
                    else
                    {
                        CMCurrent.LstCopyModels = CMNew.LstCopyModels;
                    }

                    return CMNew;
                }
                return null;
            }

            public static CardModel GenerateCopyModel(string aszCardNameLong, string aszCollectionName, List<Tuple<string, string>> aLstMetaTags)
            {
                // Really, this SCI function just parses the long name.
                MCopyObject oParsed = SCI.ConvertItemToCopyObject(aszCardNameLong);
                List<Tuple<string, string>> lstIdentifiedAttrs =
                    SCI.GetCardAttributes(aszCardNameLong);
                // We also need the rest identified attrs
                CardModel CopyM = new CardModel(
                    oParsed.Name,
                    aszCollectionName,
                    oParsed.Attributes.Select(x => new Tuple<string, string>(x.Key, x.Value)).ToList(),
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
                return ms_lstCollectionModels.FirstOrDefault(x => x.CollectionName == aszCollectionName);
            }

            public static List<string> GetLoadedCollectionList()
            {
                return ms_lstCollectionModels.Select(x => x.CollectionName).ToList();
            }

            public static List<string> GetAllCardsStartingWith(string aszStart)
            {
                return SCI.GetAllCardsStartingWith(aszStart);
            }

            public static void ImportJSONCollection()
            {
                SCI.ImportCollection();
            }

            public static void CreateCollection(string aszName)
            {
                SCI.CreateCollection(aszName);
                GenerateCollectionModel(aszName + ".txt");
            }
        }
    }
}
