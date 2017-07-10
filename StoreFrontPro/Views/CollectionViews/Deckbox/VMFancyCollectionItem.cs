using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace StoreFrontPro.Views.CollectionViews.Deckbox
{
    class VMFancyCollectionItem : ViewModel<CardModel>
    {
        public List<string> LST_TEMP_IMPORTANT_ATTRS = new List<string>()
        {
            "manaCost",
            "set"
        };

        public int Columns { get; set; } = 3;

        public ObservableCollection<UIElement> DisplayedProperties { get; set; } = new ObservableCollection<UIElement>();

        public VMFancyCollectionItem(CardModel Model, int Columns = 3) : base(Model)
        {
            this.Columns = Columns;
            SyncWithModel();
        }

        public void SyncWithModel()
        {
            DisplayedProperties.Clear();
            if (Columns != 1)
            {
                DisplayedProperties.Add(new TextBox() { Text = "x" + Model.Count, IsReadOnly = true });
                DisplayedProperties.Add(new TextBox() { Text = Model.CardName, IsReadOnly = true });
                foreach (string szKey in LST_TEMP_IMPORTANT_ATTRS)
                {
                    DisplayedProperties.Add(new TextBox() { Text = Model.GetAttr(szKey), IsReadOnly = true });
                }
            }
            else
            {
                DisplayedProperties.Add(new TextBox() { Text = Model.CardName, IsReadOnly = true });
            }
            
        }

 
    }
}
