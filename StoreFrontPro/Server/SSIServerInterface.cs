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

            public void LoadCollection(string aszCollectionFileName)
            {
                singleton.enqueueService(() =>
                {
                    inLoadCollection(aszCollectionFileName);
                });
            }

            private void inLoadCollection(string aszCollectionFileName)
            {
                string szColName = SCI.LoadCollection(aszCollectionFileName);
                if (szColName != "NF")
                {
                    inGenerateCollectionModel(szColName);
                }
            }

            /// <summary>
            /// Creates a new collection model from data from the server, if that collection model
            ///  doesn't already exist. If it exists, it updates the model.
            /// </summary>
            /// <param name="aszCollectionFileName"></param>
            /// <returns></returns>
            public void GenerateCollectionModel(string aszCollectionName)
            {
                singleton.enqueueService(() =>
                {
                    inGenerateCollectionModel(aszCollectionName);
                });
            }

            private void inGenerateCollectionModel(string aszCollectionName)
            {
                List<string> lstLoadedCollections = SCI.GetLoadedCollections();
                if (lstLoadedCollections.Contains(aszCollectionName))
                {
                    CollectionModel newCM = new CollectionModel(aszCollectionName);
                    m_lstCollectionModels.Add(newCM);
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
                // We also need the rest identified attrs
                CardModel newCopy = new CardModel(aszIdentifier, aszCollectionName);

                return newCopy;
            }

            public void SyncServerTask(Action aCallback, bool UICallback = false)
            {
                singleton.enqueueService(() => { aCallback?.Invoke(); }, UICallback);
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

            public void CreateCollection(string aszName, string aszParent = "")
            {
                if (SCI.CreateNewCollection(aszName, aszParent) != "NF")
                {
                    inGenerateCollectionModel(aszName);
                }
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
