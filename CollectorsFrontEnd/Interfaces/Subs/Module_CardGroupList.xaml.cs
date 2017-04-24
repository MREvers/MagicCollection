﻿using CollectorsFrontEnd.InterfaceModels;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
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

namespace CollectorsFrontEnd.Interfaces.Subs
{
    /// <summary>
    /// Interaction logic for CompSubCardGroupList.xaml
    /// </summary>
    public partial class Module_CardGroupList : UserControl, IComponent
    {
        #region Data Binding

        public ObservableCollection<ListViewItem> LstItems { get; set; }

        public string GroupName { get; set; }

        public Module_CardDisplayer ToolTipDisplay { get; set; }

        #endregion


        public class Data : IDataModel
        {
            public List<CardModel> LstItems = new List<CardModel>();
            public Module_CardGroupList SourceGroup;
        }

        #region Public properties

        public List<CardModel> LstCardModels = new List<CardModel>();

        public List<string> LstSortingAttrs = new List<string>() { "manaCost","name" };

        #endregion

        public event ComponentEvent UnhandledEvent;

        #region public fields
        public Data DataModel;
        #endregion

        #region Public Functions


        public Module_CardGroupList(string aszGroupName, List<CardModel> alstModels)
        {
            InitializeComponent();
            DataContext = this;

            DataModel = new Data();
            DataModel.SourceGroup = this;

            GroupName = aszGroupName + " (" + alstModels.Count + ")";
            LstCardModels = alstModels;
            LstItems = new ObservableCollection<ListViewItem>();
            for (int i = 0; i< alstModels.Count; i++)
            {
                ListViewItem newItem = new ListViewItem();
                newItem.Content = alstModels[i].GetIdealIdentifier();
                newItem.DataContext = this;
                newItem.MouseMove += eItemList_MouseMove;
                newItem.MouseLeave += eItemList_MouseLeave;
                newItem.MouseEnter += eItemList_MouseMove;
                LstItems.Add(newItem);
            }
        }

        public void ClearSelection()
        {
            CardGroupList.SelectionChanged -= eItemList_SelectionChanged;
            CardGroupList.UnselectAll();
            CardGroupList.SelectionChanged += eItemList_SelectionChanged;
        }

        public IDataModel GetDataModel()
        {
            return DataModel;
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        #endregion

        #region UI event handlers

        private void eItemList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            Data DM = (Data) GetDataModel();
            if (UnhandledEvent != null)
            {
                UnhandledEvent(DM, "Selection Changed");
            }
            
        }

        private void eItemList_MouseMove(object sender, MouseEventArgs e)
        {
            ListViewItem item = (ListViewItem)sender;
            ToolTipDisplay = null;
            string ItemText = item.Content.ToString();
            ToolTipDisplay = new Module_CardDisplayer(LstCardModels.Where(x => x.GetIdealIdentifier() == ItemText).FirstOrDefault());

        }
        private void eItemList_MouseLeave(object sender, MouseEventArgs e)
        {
            ToolTipDisplay = null;
        }

        #endregion 
    }
}