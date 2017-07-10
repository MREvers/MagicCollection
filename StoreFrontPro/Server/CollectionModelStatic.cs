using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Server
{
    public partial class CollectionModel
    {
        // This is needed so that WPF can hook on Observable collections, but gives me the ability to
        //  disable events momentarily.
        private static Dictionary<ObservableCollection<CardModel>, List<NotifyCollectionChangedEventHandler>> MAP_EVENT_TOGGLE
            = new Dictionary<ObservableCollection<CardModel>, List<NotifyCollectionChangedEventHandler>>();

        public static void RegisterCollectionListener(ObservableCollection<CardModel> myList, NotifyCollectionChangedEventHandler del)
        {
            if (!MAP_EVENT_TOGGLE.ContainsKey(myList)) { MAP_EVENT_TOGGLE[myList] = new List<NotifyCollectionChangedEventHandler>(); }
            MAP_EVENT_TOGGLE[myList].Add(del);
            myList.CollectionChanged += del;
        }

        public static void DisableEvents(ObservableCollection<CardModel> myList)
        {
            if (!MAP_EVENT_TOGGLE.ContainsKey(myList)) { MAP_EVENT_TOGGLE[myList] = new List<NotifyCollectionChangedEventHandler>(); }
            foreach (var del in MAP_EVENT_TOGGLE[myList])
            {
                myList.CollectionChanged -= del;
            }
        }

        public static void EnableEvents(ObservableCollection<CardModel> myList)
        {
            if (!MAP_EVENT_TOGGLE.ContainsKey(myList)) { MAP_EVENT_TOGGLE[myList] = new List<NotifyCollectionChangedEventHandler>(); }
            foreach (var del in MAP_EVENT_TOGGLE[myList])
            {
                myList.CollectionChanged += del;
            }
        }
    }
}
