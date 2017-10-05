using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Server
{
   partial class ServerInterface
   {

      public class CollectionIFace
      {

         public void SaveCollectionAS(string aszCollectionName)
         {
            singleton.enqueueService(() =>
            {
               SCI.SaveCollection(aszCollectionName);
            });
         }

         public void SetBaselineHistory(string aszCollection)
         {
            //SCI.SetBaselineHistory(aszCollection);
         }

         public void LoadBulkChangesAS(
             string aszCollection, List<string> alstChanges,
             Action aTask, bool UICallback = false)
         {
            singleton.enqueueService(() =>
            {
               if (alstChanges != null)
               {
                  SCI.SubmitBulkChanges(aszCollection, alstChanges);
                  aTask();
               }
            }, UICallback);

         }

         public void GetCollectionMetaDataAS(
             string aszCollectionName, Action<List<string>> aCallback,
             bool UICallback)
         {
            singleton.enqueueService(() =>
            {
               aCallback(SCI.GetCollectionMetaData(aszCollectionName));
            }, UICallback);
         }

         public List<string> GetCollectionMetaData(string aszColID)
         {
            return SCI.GetCollectionMetaData(aszColID);
         }

         public void GetCollectionListAS(
             string aszColID, bool abCollapsed,
             Action<List<string>> aCallback, bool UICallback = false)
         {
            singleton.enqueueService(() =>
            {
               aCallback(SCI.GetCollectionList(aszColID));
            }, UICallback);
         }

         public List<string> GetCollectionList(string aszColID)
         {
            return SCI.GetCollectionList(aszColID);
         }
      }

   }
}
