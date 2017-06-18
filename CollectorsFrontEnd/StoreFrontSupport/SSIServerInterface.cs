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
    public partial class ServerInterface
    {
        public class ServerIFace
        {
            private List<CollectionModel> m_lstCollectionModels = new List<CollectionModel>();

            /// <summary>
            /// Creates a new collection model from data from the server, if that collection model
            ///  doesn't already exist. If it exists, it updates the model.
            /// </summary>
            /// <param name="aszCollectionFileName"></param>
            /// <returns></returns>
            public void GenerateCollectionModel(string aszCollectionFileName)
            {
                Singleton.enqueueService(() =>
                {
                    inGenerateCollectionModel(aszCollectionFileName);
                });
            }

            private void inGenerateCollectionModel(string aszCollectionFileName)
            {
                string szColName = SCI.LoadCollection(aszCollectionFileName);
                if (szColName != "")
                {
                    // List of [ { CardNameLong, [Tags, ...] }, ... ]
                    List<Tuple<string, List<Tuple<string, string>>>> lstCards =
                        SCI.GetCollectionListWithMeta(szColName);
                    CollectionModel CMNew = new CollectionModel(szColName, lstCards);
                    CollectionModel CMCurrent = m_lstCollectionModels.FirstOrDefault(x => x.CollectionName == szColName);
                    if (CMCurrent == null)
                    {
                        m_lstCollectionModels.Add(CMNew);
                    }
                    else
                    {
                        CMCurrent.LstCopyModels = CMNew.LstCopyModels;
                    }

                }
            }

            public void GenerateCopyModel(string LongName,
                string CollectionName, 
                List<Tuple<string, string>> MetaTags, 
                Action<CardModel> Callback,
                bool UICallback = false)
            {
                Singleton.enqueueService(() =>
                {
                    Callback(inGenerateCopyModel(LongName, CollectionName, MetaTags));
                }, UICallback);
            }

            private CardModel inGenerateCopyModel(string aszCardNameLong, string aszCollectionName, List<Tuple<string, string>> aLstMetaTags)
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
            /// <param name="CollectionName"></param>
            /// <returns></returns>
            public void GetCollectionModel(string CollectionName, Action<CollectionModel> Callback, bool UICallback = false)
            {
                Singleton.enqueueService(() =>
                {
                    Callback(
                        m_lstCollectionModels.FirstOrDefault(x => x.CollectionName == CollectionName));
                }, UICallback);
            }

            public void GetLoadedCollectionList(Action<List<string>> Callback, bool UICallback = false)
            {
                Singleton.enqueueService(() =>
                {
                    Callback(SCI.GetLoadedCollections());
                }, UICallback);
            }

            public List<string> GetAllCardsStartingWith(string aszStart)
            {
                return SCI.GetAllCardsStartingWith(aszStart);
            }

            public void ImportJSONCollection()
            {
                SCI.ImportCollection();
            }

            public void CreateCollection(string aszName)
            {
                SCI.CreateCollection(aszName);
                GenerateCollectionModel(aszName + ".txt");
            }
        }
    }
}
