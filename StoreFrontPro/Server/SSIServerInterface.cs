using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Server
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
                singleton.enqueueService(() =>
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
                    List<string> lstCards = SCI.GetCollectionListWithMeta(szColName);
                    CollectionModel newCM = new CollectionModel(szColName, lstCards);
                    CollectionModel currentCM = m_lstCollectionModels.FirstOrDefault(x => x.CollectionName == szColName);
                    if (currentCM == null)
                    {
                        m_lstCollectionModels.Add(newCM);
                    }
                    else
                    {
                        currentCM.CollectionItems = newCM.CollectionItems;
                    }
                }
            }

            public void GenerateCopyModel(string Identifier,
                string CollectionName, 
                Action<CardModel> Callback,
                bool UICallback = false)
            {
                singleton.enqueueService(() =>
                {
                    Callback(inGenerateCopyModel(Identifier, CollectionName));
                }, UICallback);
            }

            private CardModel inGenerateCopyModel(string aszIdentifier, string aszCollectionName)
            {
                List<string> lstSplit = aszIdentifier.Split(':').ToList();
                string szLongName = lstSplit[0];
                string szMetaTags = "";
                if (lstSplit.Count > 0)
                {
                    szMetaTags = lstSplit[1];
                }

                List<string> lstNameAttrs = szLongName.Split('{').ToList();
                string szName = lstNameAttrs[0];
                string szDetails = "";
                if (lstNameAttrs.Count > 0)
                {
                    szDetails = lstNameAttrs[1];
                }

                // We also need the rest identified attrs
                CardModel CopyM = new CardModel(
                    szName,
                    aszCollectionName,
                    new List<Tuple<string, string>>(),
                    new List<Tuple<string, string>>(),
                    new List<Tuple<string, string>>());

                return CopyM;
            }

            public void SyncServerTask(Action aCallback)
            {
                singleton.enqueueService(() => { aCallback?.Invoke(); });
            }

            /// <summary>
            /// Returns the collection model if it exists. Null otherwise.
            /// </summary>
            /// <param name="CollectionName"></param>
            /// <returns></returns>
            public void GetCollectionModel(string CollectionName, Action<CollectionModel> Callback, bool UICallback = false)
            {
                singleton.enqueueService(() =>
                {
                    Callback(
                        GetCollectionModel(CollectionName));
                }, UICallback);
            }

            public CollectionModel GetCollectionModel(string CollectionName)
            {
                return m_lstCollectionModels.FirstOrDefault(x => x.CollectionName == CollectionName);
            }

            public void GetCollectionModels(Action<List<CollectionModel>> Callback, bool UICallback = false)
            {
                singleton.enqueueService(() =>
                {
                    Callback(GetCollectionModels());
                }, UICallback);
            }
            
            public List<CollectionModel> GetCollectionModels()
            {
                return m_lstCollectionModels;
            }

            public void GetLoadedCollectionList(Action<List<string>> Callback, bool UICallback = false)
            {
                singleton.enqueueService(() =>
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
                singleton.enqueueService(() =>
                {
                    SCI.ImportCollection();
                }, false);
            }

            public void CreateCollection(string aszName)
            {
                //SCI.CreateCollection(aszName);
                //GenerateCollectionModel(aszName + ".txt");
            }

            /// <summary>
            /// Function that does nothing. May be called to initialize the singleton.
            /// </summary>
            public void Start()
            {

            }
        }
    }
}
