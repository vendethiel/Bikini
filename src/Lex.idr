module Lex

import Control.IOExcept
import Control.Eternal

import Syntax
import XSyntax
import Complete

copenclose : String -> (Nat, Nat, Nat, String)
copenclose a =
    if isSuffixOf op ua
        then (0, sz, 2, a)
        else let cl : List Char = ['}']
             in if isSuffixOf cl ua
                    then (0, sz, 1, a)
                    else (0, sz, 0, a)
  where
    ua : List Char
    ua = unpack a
    
    op : List Char
    op = ['{']
    
    sz : Nat
    sz = length $ takeWhile (== ' ') ua

replicateX : Nat -> Nat -> Nat -> Nat -> String -> String -> String
replicateX x st s r a b =
    if x > r then (a ++ "\n" ++ b)
             else let rpl = pack $ with List replicate (st + (s * x)) ' '
                  in replicateX (x + 1) st s r a (rpl ++ "}\n" ++ b)

endComplete : (Nat, Nat, Nat, String) -> (Nat, Nat, Nat, String) -> (Nat, Nat, Nat, String)
endComplete (sc, oa, ca, a) (sb, ob, cb, b) = do
    if ca > 1
        then let stex = if ca == 3 then ob - oa
                                   else 0
                 step = if stex == 0 then sc
                                     else stex
                 s = "\n"
             in if cb == 1
                    then let diff = ((oa - ob) `div` step) - 1
                             strx = replicateX 1 ob step diff a b
                         in (step, ob, 2, strx) -- and now we check for it until the end
                    else if cb == 2 then (step, ob, ca + 1, (a ++ s ++ b))
                                    else (step, ob, ca, (a ++ s ++ b))
        else (4, ob, cb, (a ++ "\n" ++ b))

bracketBuilder : String -> String
bracketBuilder noBra =
    let strlines        = splitLines noBra
        cauto           = foldr1 complete strlines
        (_, cA)         = foldl1 completeAuto $ map (\l => ([0, 0], l)) (splitLines cauto)
        mapopen         = map copenclose (splitLines cA)
        (_, _, _, brC)  = foldl1 endComplete mapopen
    in "/* Generated by Bikini compiler */\n" ++ brC

finalize : (List BValue) -> Bool -> String
finalize v bra =
    if bra then bracketBuilder noBra
           else noBra
  where noBra : String
        noBra = concat $ map show v