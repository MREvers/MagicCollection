using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Server
{
   class CardModelBase
   {
      public string Name;
      public Dictionary<string, string> CommonAttributes;
      public Dictionary<string, List<string>> AttributeOptions;

      public CardModelBase(string CardName)
      {
         Name = CardName;
         CommonAttributes = new Dictionary<string, string>();
         AttributeOptions = new Dictionary<string, List<string>>();

         string Prototype = ServerInterface.Card.GetProtoType(Name);

         var attributePairs = CardModel.ParseTagList(Prototype);

         foreach (var Tag in attributePairs)
         {
            if( Tag.Item1.ToLower() != "name" )
            {
               CommonAttributes.Add(Tag.Item1, Tag.Item2);
            }
         }

         var optionedAttributes = new List<string>();
         var removeAttr = new List<string>();
         foreach (var Key in CommonAttributes.Keys)
         {
            string Value = CommonAttributes[Key];

            // '*' indicates that the value is restricted key.
            // TODO make this star a function from the server.
            if (Value.Contains("*"))
            {
               AttributeOptions.Add(Key, ServerInterface.Strings.ParseListDelimString(Value));
               removeAttr.Add(Key);
            }
         }

         foreach(var Key in removeAttr)
         {
            CommonAttributes.Remove(Key);
         }
      }

      public CardModel GetDefault()
      {
         List<Tuple<string,string>> lstAttrs = new List<Tuple<string, string>>();

         foreach( var pair in AttributeOptions )
         {
            lstAttrs.Add(new Tuple<string, string>(pair.Key, pair.Value.First().ToString()));
         }
         return new CardModel(Name + " " + CardModel.ToTagList(lstAttrs), "");
      }
   }

   partial class CardModel
   {
      private static Dictionary<string, CardModelBase> CardPrototypes = new Dictionary<string, CardModelBase>();

      public static CardModelBase GetPrototype(string CardName)
      {
         if (!CardPrototypes.ContainsKey(CardName))
         {
            CardModelBase newBase = new CardModelBase(CardName);
            CardPrototypes.Add(CardName, newBase);
         }

         return CardPrototypes[CardName];
      }

      public static List<Tuple<string, string>> ParseTagList(string szTags)
      {
         string szRetval = szTags.Trim(' ', '{', '}', ' ');
         List<string> lstTuples = szRetval.Split('=').ToList();
         
         List<Tuple<string, string>> lstRealTuples = new List<Tuple<string, string>>();
         if( lstTuples.Count > 0 )
         {
            string szName = lstTuples[0];
            string szVal;
            for( int i = 1; i < lstTuples.Count-1; i++ )
            {
               var keyStr = lstTuples[i].Split(new char[] { '"' }, StringSplitOptions.RemoveEmptyEntries);
               if( keyStr.Count() > 0 )
               {
                  szVal = keyStr[0];
                  lstRealTuples.Add(new Tuple<string, string>(szName.Trim(' ').Trim('*'), szVal.Trim(' ').Trim('"')));
               }

               if( keyStr.Count() == 2 )
               {
                  szName = keyStr[1];
               }
            }
            szVal = lstTuples.Last();
            lstRealTuples.Add(new Tuple<string, string>(szName.Trim(' ').Trim('*'), szVal.Trim(' ').Trim('"')));
         }

         return lstRealTuples;
      }

      public static string ToTagList(List<Tuple<string, string>> alstTagList)
      {
         string szRetval = "{ ";

         foreach(var pair in alstTagList)
         {
            szRetval += pair.Item1 + "=\"" + pair.Item2 + "\" ";
         }

         szRetval += "}";

         return szRetval;
      }

      /// <summary>
      /// Currently Unused
      /// </summary>
      /// <param name="TestModel"></param>
      /// <param name="PrototypeName"></param>
      /// <param name="LstIDs"></param>
      /// <returns></returns>
      public static bool IsRightID(CardModel TestModel, string PrototypeName, List<Tuple<string,string>> LstIDs)
      {
         bool bRetVal = PrototypeName == TestModel.PrototypeName;
         if( !bRetVal ) { return bRetVal; }

         var lstTuples = TestModel.IdentifyingAttributes.ToList();

         var lstRemoves = new List<Tuple<string,string>>();
         foreach(var id in lstTuples)
         {
            foreach(var testId in LstIDs)
            {
               if( (testId.Item1 == id.Item1) && 
                   (testId.Item2 == id.Item2) )
               {
                  lstRemoves.Add(id);
                  break;
               }
            }
         }

         foreach( var remId in lstRemoves )
         {
            lstTuples.Remove(remId);
         }

         bRetVal &= lstTuples.Count == 0;

         return bRetVal;
      }
   }
}
