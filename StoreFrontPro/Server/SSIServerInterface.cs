using StoreFrontPro.Views;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Animation;

namespace StoreFrontPro.Server
{
   /// <summary>
   /// File Name explanation.
   /// S => Static
   /// SI => Server Interface
   /// </summary>
   partial class ServerInterface
   {
      public class ServerIFace
      {
         #region Config
         public string GetImagesFolderPath()
         {
            return SCI.GetImagesPath();
         }

         public string GetImportSourceFilePath()
         {
            return SCI.GetImportSourceFilePath();
         }

         public string GetSourceFilePath()
         {
            return SCI.GetSourceFilePath();
         }
         #endregion


         // It is expected that any object referenced in these actions
         // survives to running time of the application.
         private List<Action> Notifiers;
         public List<CollectionModel> Collections;

         public ServerIFace()
         {
            Collections = new List<CollectionModel>();
            Notifiers = new List<Action>();
         }

         /// <summary>
         /// Notified whenever a collection model is created.
         /// </summary>
         /// <param name="aAction"></param>
         public void RegisterChangeListener(Action aAction)
         {
            Notifiers.Add(aAction);
         }

         private void notify()
         {
            Notifiers.ForEach(x => x?.Invoke());
         }

         public void LoadCollectionAS(string aszCollectionFileName)
         {
            singleton.enqueueService(() =>
            {
               inLoadCollection(aszCollectionFileName);
            });
         }

         private void inLoadCollection(string aszCollectionFileName)
         {
            string szColID = SCI.LoadCollection(aszCollectionFileName);
            if (szColID != "NF")
            {
               inGenerateCollectionModel(szColID);
            }
         }

         /// <summary>
         /// Creates a new collection model from data from the server, if that collection model
         ///  doesn't already exist. If it exists, it updates the model.
         /// </summary>
         /// <param name="aszCollectionFileName"></param>
         /// <returns></returns>
         public void GenerateCollectionModelAS(string aszCollectionName)
         {
            singleton.enqueueService(() =>
            {
               inGenerateCollectionModel(aszCollectionName);
            });
         }

         private void inGenerateCollectionModel(string aszCollectionID)
         {
            List<string> lstLoadedCollections = SCI.GetLoadedCollections();
            if ((lstLoadedCollections.Contains(aszCollectionID)) &&
                (!Collections.Select(x => x.ID).Contains(aszCollectionID)))
            {
               createCollectionModel(aszCollectionID);
            }
         }

         private void createCollectionModel(string aszCollectionID)
         {
            CollectionModel newCM = new CollectionModel(aszCollectionID);
            Collections.Add(newCM);
            notify();
         }

         public void GenerateCopyModelAS(string Identifier, string CollectionName,
                                          Action<CardModel> Callback, bool UICallback = false)
         {
            GenerateCopyModelsAS(new List<string>() { Identifier }, CollectionName,
                                  (lst) => { Callback(lst.FirstOrDefault()); }, UICallback);
         }

         public void GenerateCopyModelsAS(List<string> Identifiers, string CollectionName,
                                           Action<List<CardModel>> Callback, bool UICallback = false)
         {
            singleton.enqueueService(() =>
            {
               Callback?.Invoke(Identifiers.Select((x) => { return createCopyModel(x, CollectionName); }).ToList());
            }, UICallback);
         }

         private CardModel createCopyModel(string aszIdentifier, string aszCollectionName)
         {
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
         public void GetCollectionModelAS(string CollectionName, Action<CollectionModel> Callback, bool UICallback = false)
         {
            singleton.enqueueService(() =>
            {
               Callback(GetCollectionModel(CollectionName));
            }, UICallback);
         }

         public CollectionModel GetCollectionModel(string CollectionName)
         {
            return Collections.FirstOrDefault(x => x.CollectionName == CollectionName);
         }

         public void GetCollectionModelsAS(Action<List<CollectionModel>> Callback, bool UICallback = false)
         {
            singleton.enqueueService(() =>
            {
               Callback(getCollectionModels());
            }, UICallback);
         }

         private List<CollectionModel> getCollectionModels()
         {
            List<string> lstServerCollections = SCI.GetLoadedCollections();
            foreach (string szColID in lstServerCollections)
            {
               if (!Collections.Select(x => x.ID).Contains(szColID))
               {
                  createCollectionModel(szColID);
               }
            }

            return Collections;
         }

         public void GetLoadedCollectionListAS(Action<List<string>> Callback, bool UICallback = false)
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

         public void ImportJSONCollectionAS()
         {
            singleton.enqueueService(() =>
            {
               ImportJSONCollection();
            }, false, true);
         }

         public void ImportJSONCollection()
         {
            SCI.ImportCollection();
         }

         public void CreateCollection(string aszName, string aszParent = "")
         {
            string szID = SCI.CreateNewCollection(aszName, aszParent);
            if (szID != "NF")
            {
               inGenerateCollectionModel(szID);
            }
         }
      }
   }
}
