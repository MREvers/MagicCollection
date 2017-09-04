using StoreFrontPro.Server;
using StoreFrontPro.Tools;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace StoreFrontPro.Views.CollectionViews.Cube
{
   class VMCardGroupDisplay : ViewModel<BasicModel<List<CardModel>>>
   {
      public ObservableCollection<VCardGroupList> CategoryGroups { get; set; } = 
         new ObservableCollection<VCardGroupList>() { };

      // We can come up with a better solution in the future
      private List<string> m_lstPriorityGroupingKeys = new List<string>() { "W", "U", "B", "R", "G" };

      public VMCardGroupDisplay(BasicModel<List<CardModel>> Model, string RoutingName) : base(Model, RoutingName)
      {
         Model.Register(this);

         ModelUpdated();
      }

      private void inSyncWithModel()
      {
         // Get all of the grouping keys
         List<string> lstGroupingKeys = getCurrentGroupingKeys();

         // Remove groups that no longer exist.
         List<VCardGroupList> lstRemoveGroups = new List<VCardGroupList>();
         foreach( var group in CategoryGroups )
         {
            MCardGroupList groupModel = getCardGroupModelFromView(group);
            if( groupModel.GroupedList.Count == 0 )
            {
               lstRemoveGroups.Add(group);
            }
         }

         foreach (var rmGroup in lstRemoveGroups)
         {
            VMCardGroupList rmGroupVM = (rmGroup.DataContext as VMCardGroupList);
            rmGroupVM.FreeModel();
            CategoryGroups.Remove(rmGroup);
         }

         // Add groups that are new.
         foreach (string category in lstGroupingKeys.Where(x => !hasDisplay(x)))
         {
            ViewClass oCardGroupClass = ViewFactory.CreateCardGroupList("manaCost", category, m_lstPriorityGroupingKeys, Model);
            if( oCardGroupClass != null )
            {
               CategoryGroups.Add((VCardGroupList)oCardGroupClass.View);
               sortCategoryGroups();
            }
         }
      }

      /// <summary>
      /// Returns true if this group display is already displaying the input category.
      /// </summary>
      /// <param name="aszCategory"></param>
      /// <returns></returns>
      private bool hasDisplay(string aszCategory)
      {
         VCardGroupList oExistingDisplay = CategoryGroups
            .Where(x => ((VMCardGroupList)(x.DataContext)).GroupName == aszCategory)
            .FirstOrDefault();
         return oExistingDisplay != null;
      }

      /// <summary>
      /// Returns the list of grouping keys currently present in the 
      /// model.
      /// </summary>
      /// <returns></returns>
      private List<string> getCurrentGroupingKeys()
      {
         return Model.Item.Select(x => getCategory(x.GetAttr("manaCost"))).Distinct().ToList();
      }

      private string getCategory(string szMan)
      {
         string szRetVal = "";

         foreach (string szPriorityCategory in m_lstPriorityGroupingKeys.Where(x => szMan.Contains(x)))
         {
            szRetVal += szPriorityCategory + "\\";
         }

         // Delete the trailing \
         int iLastSlash = szRetVal.LastIndexOf('\\');
         szRetVal = szRetVal.Substring(0, iLastSlash);

         return szRetVal;
      }

      private void sortCategoryGroups()
      {
         List<VCardGroupList> lstTemp = CategoryGroups.ToList();

         CategoryGroups.Clear();
         foreach (string category in m_lstPriorityGroupingKeys)
         {
            VCardGroupList lst = lstTemp.Where(x => ((VMCardGroupList)x.DataContext).GroupName == category)
                                        .FirstOrDefault();
            if (lst != null)
            {
               CategoryGroups.Add(lst);
               lstTemp.Remove(lst);
            }
         }

         foreach (VCardGroupList VList in lstTemp)
         {
            string szGroup = ((VMCardGroupList)VList.DataContext).GroupName;

            int i = szGroup.Length > 0 ? 0 : CategoryGroups.Count();
            for (; i < CategoryGroups.Count(); i++)
            {
               string inListGroup = ((VMCardGroupList)CategoryGroups[i].DataContext).GroupName;
               if (szGroup.Length <= inListGroup.Length)
               {
                  break;
               }
            }

            if (i == CategoryGroups.Count() - 1)
            {
               CategoryGroups.Add(VList);
            }
            else
            {
               CategoryGroups.Insert(i, VList);
            }
         }
      }

      private MCardGroupList getCardGroupModelFromView(VCardGroupList view)
      {
         return ((VMCardGroupList)view?.DataContext)?.Model;
      }

      #region IViewModel
      public override void ModelUpdated()
      {
         Action actWrapper = () => { ModelUpdated(); };
         if (!Application.Current.Dispatcher.CheckAccess())
         {
            Application.Current.Dispatcher.BeginInvoke(actWrapper);
            return;
         }
         inSyncWithModel();
      }
      #endregion
   }
}
