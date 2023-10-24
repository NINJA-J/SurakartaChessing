import React, {useCallback, useEffect, useRef, useState} from 'react';
import axios from "axios";

const EMPTY = 0;
const PLAYER = 1;
const OPPONENT = 2;

const EMPTY_FROM = 3;
const EMPTY_TO = 4;
const PLAYER_FROM = 5;
const PLAYER_TO = 6;
const OPPONENT_FROM = 7;
const OPPONENT_TO = 8;

const ChessBoard = ({
                        chessboard,
                        vSessionId,
                        vIsPlayer
                    }) => {
    const [press, setPress] = useState(null)
    const [compMove, setCompMove] = useState(null)
    const [playerMove, setPlayerMove] = useState(null)
    const [chess, setChess] = useState(chessboard == null ? [
        [OPPONENT, OPPONENT, OPPONENT, OPPONENT, OPPONENT, OPPONENT],
        [OPPONENT, OPPONENT, OPPONENT, OPPONENT, OPPONENT, OPPONENT],
        [EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY],
        [EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY],
        [PLAYER, PLAYER, PLAYER, PLAYER, PLAYER, PLAYER],
        [PLAYER, PLAYER, PLAYER, PLAYER, PLAYER, PLAYER]
    ] : chessboard)

    const waitOpponent = useRef(false)
    const from = useRef(null)
    const availableMoves = useRef([])
    const isPlayer = useRef(vIsPlayer == null ? true : vIsPlayer)
    const sessionId = useRef(vSessionId)

    useEffect(() => {
        if (sessionId.current == null && chess.length > 0) {
            axios.post(`${process.env.REACT_APP_BACKEND_URL}/session/register`, {
                rawBoard: chess,
                isPlayerTurn: isPlayer.current,
            }).then((response) => {
                response = response.data;
                if (response.code !== 0) {
                    console.error(response.message)
                } else {
                    const {sessionId: _sessionId, nextAvailableMoves} = response.data
                    nextAvailableMoves.forEach((e) => delete e['arc'])
                    console.info(`Set session ID to ${_sessionId}`)
                    sessionId.current = _sessionId
                    availableMoves.current = nextAvailableMoves
                }
            })
        }
    }, [sessionId, chess])

    const moveEquals = useCallback((m1, m2) => {
        return m1.from.x === m2.from.x && m1.from.y === m2.from.y && m1.to.x === m2.to.x && m1.to.y === m2.to.y
    }, [])

    useEffect(() => {
        console.info(`Call useEffect on compMove ${JSON.stringify(compMove)}`)
        const tmpChess = chess.slice()
        if (playerMove != null) {
            console.info(`Update playerMove ${JSON.stringify(playerMove)} in compMove ${JSON.stringify(compMove)}`)
            tmpChess[playerMove.from.x][playerMove.from.y] = EMPTY;
            tmpChess[playerMove.to.x][playerMove.to.y] = PLAYER;
            setChess(tmpChess)
        }
        if (compMove != null) {
            console.info(`Update chess in compMove ${JSON.stringify(compMove)}`)
            tmpChess[compMove.from.x][compMove.from.y] = EMPTY_FROM;
            tmpChess[compMove.to.x][compMove.to.y] = OPPONENT_TO;
            setChess(tmpChess)
        }
    }, [compMove])

    useEffect(() => {
        console.info("Call useEffect on playerMove")
        if (playerMove == null) {
            console.info("Call useEffect on playerMove")
            return
        }
        const tmpChess = chess.slice()
        if (compMove != null) {
            tmpChess[compMove.from.x][compMove.from.y] = EMPTY;
            tmpChess[compMove.to.x][compMove.to.y] = OPPONENT;
        }

        tmpChess[playerMove.from.x][playerMove.from.y] = PLAYER_FROM;
        if (tmpChess[playerMove.to.x][playerMove.to.y] === EMPTY) {
            tmpChess[playerMove.to.x][playerMove.to.y] = EMPTY_TO;
        } else if (tmpChess[playerMove.to.x][playerMove.to.y] === OPPONENT) {
            tmpChess[playerMove.to.x][playerMove.to.y] = OPPONENT_TO;
        }
        setChess(tmpChess)
        axios.post(`${process.env.REACT_APP_BACKEND_URL}/session/${sessionId.current}/move`, {
            playerMove: playerMove,
            doComputerMove: true,
        }).then((response) => {
            response = response.data;
            if (response.code !== 0) {
                console.error(response.message)
            } else {
                const {compMove: _compMove, nextAvailableMoves} = response.data
                console.info(`Response move ${JSON.stringify(response.data)}`)
                setCompMove(_compMove)
                nextAvailableMoves.forEach((e) => delete e['arc'])
                availableMoves.current = nextAvailableMoves
                waitOpponent.current = false
            }
        })
    }, [playerMove])

    useEffect(() => {
        console.info("Call useEffect on press")
        if (waitOpponent.current || press == null) {
            return
        }
        const tmpChess = chess.slice()
        if (from.current == null) {
            if (tmpChess[press.x][press.y] !== PLAYER) {
                console.info(`Skip from ${press.x} ${press.y}`)
                return;
            }
            console.info(`Set from ${press.x} ${press.y}`)
            tmpChess[press.x][press.y] = PLAYER_FROM;
            setChess(tmpChess)
            from.current = press
        } else {
            const move = {
                from: from.current,
                to: press
            }
            if (!availableMoves.current.some((m) => moveEquals(m, move))) {
                console.warn(`Invalid move ${JSON.stringify(move)} ${JSON.stringify(availableMoves.current)}`)
                tmpChess[from.current.x][from.current.y] = PLAYER;
                setChess(tmpChess)
            } else {
                setPlayerMove(move)
            }

            from.current = null
        }
    }, [press])

    return (<div className="chess-board">
        {chess.flatMap((chessRow, i) => chessRow.map((type, j) => {
                if (type === PLAYER || type === EMPTY) {
                    return <Chess onSelect={() => setPress({x: i, y: j})} type={type} x={i} y={j}/>
                } else {
                    return <Chess type={type} x={i} y={j}/>
                }
            })
        )}
    </div>);
}

const Chess = ({
                   type,
                   onSelect,
                   x, y
               }) => {
    const [stressed, setStressed] = useState(false)

    const yl = -0.8, yr = -1.7, xl = -1, xr = -1;

    let realSize = 0.85;
    let style = {}
    switch (type) {
        case PLAYER:
            style = {backgroundColor: "blue"};
            realSize = stressed ? 0.85 : 0.7;
            break;
        case PLAYER_FROM:
            style = {backgroundColor: "blue", board: "5px dashed greenyellow"};
            break;
        case PLAYER_TO:
            style = {backgroundColor: "blue", board: "5px solid red"};
            break;
        case OPPONENT:
            style = {backgroundColor: "yellow"};
            realSize = stressed ? 0.85 : 0.7;
            break;
        case OPPONENT_FROM:
            style = {backgroundColor: "yellow", board: "5px dashed greenyellow"};
            break;
        case OPPONENT_TO:
            style = {backgroundColor: "yellow", board: "5px solid red"};
            break;
        case EMPTY:
            style = {backgroundColor: "gray", opacity: stressed ? 0.75 : 0};
            realSize = 0.7;
            break;
        case EMPTY_FROM:
            style = {backgroundColor: "gray", opacity: 0.75, board: "5px dashed greenyellow"};
            realSize = 0.7;
            break;
        case EMPTY_TO:
            style = {backgroundColor: "gray", opacity: 0.75, board: "5px solid red"};
            realSize = 0.7;
            break;
        default:
            break;
    }

    style = {
        ...style,
        position: 'absolute',
        top: "" + ((x + 2 - realSize / 2) / 9 * (100 - xl - xr) + xl) + "%",
        left: "" + ((y + 2 - realSize / 2) / 9 * (100 - yl - yr) + yl) + "%",
        height: "" + (realSize / 9 * 100) + "%",
        width: "" + (realSize / 9 * 100) + "%",
    }

    return (<div className="chess"
                 style={style}
                 onClick={() => onSelect && onSelect()}
                 onMouseOver={() => setStressed(true)}
                 onMouseOut={() => setStressed(false)}>
    </div>);
}

export default ChessBoard;