using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Tools
{
    public static class Extensions
    {
        public static void ForEach<T>(this ObservableCollection<T> myList, Action<T> action)
        {
            foreach(T item in myList)
            {
                action(item);
            }
        }
    }
}
