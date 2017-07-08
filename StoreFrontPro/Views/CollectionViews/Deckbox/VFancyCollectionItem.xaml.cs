using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace StoreFrontPro.Views.CollectionViews.Deckbox
{
    /// <summary>
    /// Interaction logic for VFancyCollectionItem.xaml
    /// </summary>
    public partial class VFancyCollectionItem : UserControl, INotifyPropertyChanged
    {

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public VFancyCollectionItem()
        {
            InitializeComponent();
            DataContextChanged += (o, e) =>
            {
                formatGrid();
            };
        }

        private void formatGrid()
        {
            if (DataContext == null) { return; }

            VMFancyCollectionItem context = (VMFancyCollectionItem)this.DataContext;

            MainGrid.ColumnDefinitions.Clear();

            int iColumns = context.Columns;
            for (int i = 0; i < iColumns; i++)
            {
                MainGrid.ColumnDefinitions.Add( new ColumnDefinition() { } );
                UIElement tbItem = context.DisplayedProperties[i];
                MainGrid.Children.Add(tbItem);
                Grid.SetColumn(tbItem, i);
            }
        }
    }
}
