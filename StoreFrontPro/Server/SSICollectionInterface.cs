﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Server
{
    public partial class ServerInterface
    {

        public class CollectionIFace
        {
            public void SaveCollection(string aszCollectionName)
            {
                singleton.enqueueService(() =>
                {
                    SCI.SaveCollection(aszCollectionName);
                });
            }

            public void AddItem(string aszCollectionName, string aszCardNameLong, List<Tuple<string, string>> lstMeta)
            {
                //SCI.AddItem(aszCollectionName, aszCardNameLong, lstMeta);
            }

            public void RemoveItem(string aszCollectionName, string aszCardNameLong, List<Tuple<string, string>> lstMeta)
            {
                //SCI.RemoveItem(aszCollectionName, aszCardNameLong, lstMeta);
            }

            public void SetBaselineHistory(string aszCollection)
            {
                //SCI.SetBaselineHistory(aszCollection);
            }

            public void LoadBulkChanges(string aszCollection, List<string> alstChanges, Action aTask, bool UICallback = false)
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

            public void GetCollectionList(string aszCollectionName, bool abCollapsed, Action<List<string>> aCallback, bool UICallback = false)
            {
                singleton.enqueueService(() =>
                {
                    aCallback(SCI.GetCollectionList(aszCollectionName, abCollapsed));
                }, UICallback);
            }
        }

    }
}
