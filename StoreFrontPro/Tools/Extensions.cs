using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

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
