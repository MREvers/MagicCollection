using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Server
{
   public class CardModelBase
   {
      public Dictionary<string, string> CommonAttributes;
      public Dictionary<string, List<string>> AttributeOptions;

      public CardModelBase(string CardName)
      {
         CommonAttributes = new Dictionary<string, string>();
         AttributeOptions = new Dictionary<string, List<string>>();

         string Prototype = ServerInterface.Card.GetProtoType(CardName);

         var attributePairs = CardModel.ParseTagList(Prototype);

         foreach (var Tag in attributePairs)
         {
            CommonAttributes.Add(Tag.Item1, Tag.Item2);
         }

         var optionedAttributes = new List<string>();
         foreach (var Key in CommonAttributes.Keys)
         {
            string Value = CommonAttributes[Key];

            // '*' indicates that the value is restricted key.
            if (Value.Contains("*"))
            {
               optionedAttributes.Add(Key);
            }
         }

         foreach (var Key in optionedAttributes)
         {
            string Value = CommonAttributes[Key];

            // Remove '*'
            Value = Value.Substring(1);
            List<string> lstSplitOps = Value.Split(new string[] { "::" }, StringSplitOptions.RemoveEmptyEntries)
                                            .ToList();

            AttributeOptions.Add(Key, lstSplitOps);

            CommonAttributes.Remove(Key);
         }
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
         List<string> lstTuples = szRetval.Split(' ').ToList();
         List<Tuple<string, string>> lstRealTuples = new List<Tuple<string, string>>();
         foreach (string tup in lstTuples)
         {
            List<string> lstKeyAndVal = tup.Split('=').ToList();
            if (lstKeyAndVal.Count == 2)
            {
               string szKey = lstKeyAndVal[0].Trim(' ');
               string szVal = lstKeyAndVal[1].Trim(' ', '\"');
               lstRealTuples.Add(new Tuple<string, string>(szKey, szVal));
            }
         }

         return lstRealTuples;
      }
   }
}
